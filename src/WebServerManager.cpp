#include "WebServerManager.hpp"

#include <vector>
#include <ArduinoJson.h>
#include <FS.h>

WebServerManager::WebServerManager(
  EmotionState &emotionState, 
  FanController &fanController,
  EarController &earController, 
  TiltController &tiltController,
  AnimationManager &animationManager
)
    : server_(80),
      emotionState_(emotionState),
      fanController_(fanController),
      earController_(earController),
      tiltController_(tiltController),
      animationManager_(animationManager) {}

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
        auto *context = static_cast<UploadContext *>(request->_tempObject);
        if (context == nullptr) {
          request->send(400, "text/plain", F("No file uploaded."));
          return;
        }

        String message = context->message;
        if (!message.length()) {
          message = F("File was saved.");
        }

        if (context->error) {
          request->send(400, "text/plain", message);
        } else {
          request->send(200, "text/plain", message);
        }

        delete context;
        request->_tempObject = nullptr;
      },
      [this](AsyncWebServerRequest *request, String filename, size_t index,
             uint8_t *data, size_t len, bool final) {
            Serial.println(F("[I] File upload callback called"));
        handleFileUpload(request, filename, index, data, len, final);
      });

  server_.on("/file", HTTP_DELETE, [this](AsyncWebServerRequest *request) {
    if (!request->hasParam("file")) {
      request->send(400, "text/plain", F("Parameter 'file' not present!"));
      return;
    }

    auto filePath = request->getParam("file")->value();
    Serial.println("[I] Deleting " + String(filePath));

    if(!SPIFFS.exists(filePath)){
      request->send(400, "text/plain", F("File does not exist!"));
      return;
    }

    if(!filePath.startsWith("/anims/")){
      request->send(400, "text/plain", F("Cannot delete the file, file is not an animation!"));
      return;
    }

    SPIFFS.remove(filePath);
    request->send(200, "text/plain", F("File was deleted."));
  });

  server_.on("/files", HTTP_GET, [this](AsyncWebServerRequest *request) {
    auto emotions = animationManager_.getEmotions();

    JsonDocument doc;
    JsonArray array = doc.to<JsonArray>();
    for (const auto emotion : emotions) {
      array.add(emotion.path);
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

void WebServerManager::handleFileUpload(AsyncWebServerRequest *request,
                                        String filename, size_t index,
                                        uint8_t *data, size_t len,
                                        bool final) {
  auto *context = static_cast<UploadContext *>(request->_tempObject);

  if (index == 0) {
    if (context != nullptr) {
      delete context;
      context = nullptr;
    }

    context = new UploadContext();
    request->_tempObject = context;

    filename.trim();
    if (filename.isEmpty()) {
      context->error = true;
      context->message = F("File name is required.");
      return;
    }

    for (size_t i = 0; i < filename.length(); ++i) {
      char c = filename.charAt(i);
      if (c == '/' || c == '\\') {
        filename.setCharAt(i, '_');
      }
    }

    context->targetPath = "/anims/" + filename;
    context->tempPath = context->targetPath + F(".tmp");

    if (SPIFFS.exists(context->tempPath)) {
      SPIFFS.remove(context->tempPath);
    }

    request->_tempFile = SPIFFS.open(context->tempPath, FILE_WRITE);
    if (!request->_tempFile) {
      context->error = true;
      context->message = F("Error opening temporary file for writing!");
      return;
    }
  }

  if (context == nullptr) {
    return;
  }

  if (context->error) {
    if (final && request->_tempFile) {
      request->_tempFile.close();
      request->_tempFile = File();
    }
    return;
  }

  if (len > 0 && request->_tempFile) {
    size_t written = request->_tempFile.write(data, len);
    if (written != len && !context->error) {
      context->error = true;
      context->message = F("Failed to write uploaded data.");
      request->_tempFile.close();
      request->_tempFile = File();
      if (SPIFFS.exists(context->tempPath)) {
        SPIFFS.remove(context->tempPath);
      }
    }
  }

  if (!final) {
    return;
  }

  if (request->_tempFile) {
    request->_tempFile.close();
    request->_tempFile = File();
  }

  if (context->error) {
    if (SPIFFS.exists(context->tempPath)) {
      SPIFFS.remove(context->tempPath);
    }
    return;
  }

  if (SPIFFS.exists(context->targetPath)) {
    SPIFFS.remove(context->targetPath);
  }

  if (!SPIFFS.rename(context->tempPath, context->targetPath)) {
    context->error = true;
    context->message = F("Failed to save file.");
    if (SPIFFS.exists(context->tempPath)) {
      SPIFFS.remove(context->tempPath);
    }
    return;
  }

  context->message = F("File was saved.");
}
