#include "WebServerManager.hpp"

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

  server_.on("/file", HTTP_POST, [this](AsyncWebServerRequest *request) {
    if (request->hasParam("file", true) && request->hasParam("content", true)) {
      String path = "/anims/" + request->getParam("file", true)->value();
      File file = SPIFFS.open(path, "w");
      if (!file) {
        Serial.println(F("[E] There was an error opening the file for saving an animation!"));
        file.close();
        request->send(400, "text/plain", F("Error opening file for writing!"));
      } else {
        Serial.println(F("[I] File saved!"));
        file.print(request->getParam("content", true)->value());
        file.close();
        request->send(200, "text/plain", F("File was saved."));
      }
    } else {
      request->send(400, "text/plain", F("No valid parameters detected!"));
    }
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
    request->send(200, "text/plain", F("[]"));
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
    request->send(200, "text/plain", earController_.getColorHexString());
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
