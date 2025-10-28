#include "WebServerManager.hpp"

#include <vector>
#include <ArduinoJson.h>

namespace {
struct UploadContext {
  String tempPath;
  String finalPath;
  bool error = false;
  int status = 200;
  String message;
  size_t received = 0;
};

uint8_t uploadHandledTag;
void *const kUploadHandled = &uploadHandledTag;

String sanitizeFileName(String name) {
  name.trim();

  String sanitized;
  sanitized.reserve(name.length());
  for (size_t i = 0; i < name.length(); ++i) {
    char c = name.charAt(i);
    bool allowed = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                   (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.';
    if (allowed) {
      sanitized += c;
    }
  }

  if (sanitized.length() == 0) {
    sanitized = F("animation");
  }

  String lower = sanitized;
  lower.toLowerCase();
  if (!lower.endsWith(F(".gif"))) {
    sanitized += F(".gif");
  }

  if (sanitized.length() > 0 && sanitized.charAt(0) == '.') {
    sanitized = String(F("animation")) + sanitized;
  }

  return sanitized;
}
} // namespace

WebServerManager::WebServerManager(EmotionState &emotionState, FanController &fanController,
                                   EarController &earController, TiltController &tiltController)
    : server_(80),
      emotionState_(emotionState),
      fanController_(fanController),
      earController_(earController),
      tiltController_(tiltController) {}

void WebServerManager::begin(const char *ssid, const char *password) {
  WiFi.softAP(ssid, password);

  registerRoutes();

  ElegantOTA.begin(&server_);
  server_.begin();
  ElegantOTA.setAutoReboot(true);
}

void WebServerManager::loop() {
  ElegantOTA.loop();
}

void WebServerManager::registerRoutes() {
  server_.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server_.on(
      "/file", HTTP_POST,
      [](AsyncWebServerRequest *request) {
        if (request->_tempObject == kUploadHandled) {
          request->_tempObject = nullptr;
        }
      },
      [this](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data,
             size_t len, bool final) {
        handleFileUpload(request, filename, index, data, len, final);
      });

  server_.on("/file", HTTP_DELETE, [this](AsyncWebServerRequest *request) {
    if (request->hasParam("file")) {
      String path = "/anims/" + request->getParam("file")->value();
      SPIFFS.remove(path);
      request->send(200, "text/plain", F("File was deleted."));
    } else {
      request->send(400, "text/plain", F("Parameter 'file' not present!"));
    }
  });

  server_.on("/files", HTTP_GET, [this](AsyncWebServerRequest *request) {
    File dir = SPIFFS.open("/anims");
    if (!dir || !dir.isDirectory()) {
      if (dir) {
        dir.close();
      }
      request->send(500, "application/json", F("[]"));
      return;
    }

    std::vector<String> fileNames;
    fileNames.reserve(8);

    File file = dir.openNextFile();
    while (file) {
      if (!file.isDirectory()) {
        String name = file.name();
        int slashIndex = name.lastIndexOf('/');
        if (slashIndex >= 0) {
          name = name.substring(slashIndex + 1);
        }

        if (name.endsWith(".gif")) {
          fileNames.emplace_back(std::move(name));
        }
      }
      file.close();
      file = dir.openNextFile();
    }
    dir.close();

    JsonDocument doc;
    JsonArray array = doc.to<JsonArray>();
    for (const String &fileName : fileNames) {
      array.add(fileName);
    }

    String json;
    serializeJson(array, json);
    request->send(200, "application/json", json);
  });

  server_.on("/emotion", HTTP_GET, [this](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", emotionState_.getCurrentEmotion());
  });

  server_.on("/emotion", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    if (request->hasParam("name", true)) {
      emotionState_.setCurrentEmotion(request->getParam("name", true)->value());
      request->send(200, "text/plain", F("Emotion changed."));
    } else {
      request->send(400, "text/plain", F("No valid parameters detected!"));
    }
  });

  server_.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });

  server_.on("/fan", HTTP_GET, [this](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(fanController_.getDutyCycle()));
  });

  server_.on("/fan", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    if (request->hasParam("duty", true)) {
      int duty = request->getParam("duty", true)->value().toInt();
      if (fanController_.setDutyCycle(duty)) {
        request->send(200, "text/plain", "Set PWM to: " + String(fanController_.getDutyCycle()));
        return;
      }
    }
    request->send(400, "text/plain", F("Invalid duty cycle"));
  });

  server_.on("/ears", HTTP_GET, [this](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", earController_.getColorHexString()+ F(" ") + earController_.getBrightness());
  });

  server_.on("/ears", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    if (request->hasParam("color", true)) {
      String color = request->getParam("color", true)->value();
      int r = 0, g = 0, b = 0;
      int itemsFound = sscanf(color.c_str(), "#%02x%02x%02x", &r, &g, &b);
      if (itemsFound != 3) {
        request->send(400, "text/plain", F("Could not set color use #FFFFFF format."));
        return;
      }
      earController_.setColor(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b));
    }
    if (request->hasParam("brightness", true)) {
      int brightness = request->getParam("brightness", true)->value().toInt();
      if (brightness >= 256 || brightness < 0) {
        request->send(400, "text/plain", F("Could not set brightness use 0-255 value."));
        return;
      }
      earController_.setBrightness(static_cast<uint8_t>(brightness));
    }
    request->send(200, "text/plain", F("Color/brightness set."));
  });

  server_.on("/gyro", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if (!tiltController_.isEnabled()) {
      request->send(200, "text/plain", F("Tilt is disabled"));
      return;
    }
    request->send(200, "text/plain", tiltController_.readAcceleration());
  });

  server_.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
  });
}

void WebServerManager::handleFileUpload(AsyncWebServerRequest *request, const String &filename,
                                        size_t index, uint8_t *data, size_t len, bool final) {
  auto *context = static_cast<UploadContext *>(request->_tempObject);

  if (index == 0) {
    String desiredName = filename;
    if (request->hasParam("name", true)) {
      desiredName = request->getParam("name", true)->value();
    }

    String sanitized = sanitizeFileName(desiredName);
    if (!context) {
      context = new UploadContext();
      request->_tempObject = context;
    }

    context->error = false;
    context->status = 200;
    context->message = F("File was saved.");
    context->received = 0;
    context->finalPath = "/anims/" + sanitized;
    context->tempPath = context->finalPath + F(".tmp");

    if (request->_tempFile) {
      request->_tempFile.close();
      request->_tempFile = File();
    }

    SPIFFS.remove(context->tempPath);

    request->_tempFile = SPIFFS.open(context->tempPath, FILE_WRITE);
    if (!request->_tempFile) {
      context->error = true;
      context->status = 500;
      context->message = F("Failed to open temporary file.");
      Serial.println(F("[E] Failed to open temporary file for upload."));
      return;
    }
  }

  if (!context) {
    Serial.println(F("[E] Upload context missing."));
    return;
  }

  if (len && request->_tempFile && !context->error) {
    size_t written = request->_tempFile.write(data, len);
    if (written != len) {
      context->error = true;
      context->status = 500;
      context->message = F("Failed to write uploaded data.");
      Serial.println(F("[E] Failed to write uploaded data."));
    }
    context->received += written;
  }

  if (final && context->received == 0 && !context->error) {
    context->error = true;
    context->status = 400;
    context->message = F("No file data received.");
  }

  if (final) {
    if (request->_tempFile) {
      request->_tempFile.close();
      request->_tempFile = File();
    }

    if (context->error) {
      SPIFFS.remove(context->tempPath);
    } else {
      SPIFFS.remove(context->finalPath);
      if (!SPIFFS.rename(context->tempPath, context->finalPath)) {
        context->error = true;
        context->status = 500;
        context->message = F("Failed to finalize uploaded file.");
        SPIFFS.remove(context->tempPath);
        Serial.println(F("[E] Failed to finalize uploaded file."));
      } else {
        Serial.println(F("[I] File uploaded!"));
      }
    }

    int status = context->error ? context->status : 200;
    String message =
        context->error ? (context->message.length() ? context->message
                                                    : String(F("Failed to save file.")))
                        : context->message;
    request->send(status, "text/plain", message);

    delete context;
    request->_tempObject = kUploadHandled;
  }
}
