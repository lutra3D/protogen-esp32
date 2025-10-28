#ifndef WEB_SERVER_MANAGER_HPP
#define WEB_SERVER_MANAGER_HPP

#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>
#include <SPIFFS.h>
#include <WiFi.h>

#include <Arduino.h>

#include "EarController.hpp"
#include "EmotionState.hpp"
#include "FanController.hpp"
#include "TiltController.hpp"
#include "AnimationManager.hpp"

class WebServerManager {
public:
  WebServerManager(EmotionState &emotionState, FanController &fanController,
                   EarController &earController, TiltController &tiltController,
                   AnimationManager &animationManager);

  void begin(const char *ssid, const char *password);
  void loop();

private:
  void registerRoutes();
  void handleFileUpload(AsyncWebServerRequest *request, const String &filename,
                        size_t index, uint8_t *data, size_t len, bool final);

  AsyncWebServer server_;
  EmotionState &emotionState_;
  FanController &fanController_;
  EarController &earController_;
  TiltController &tiltController_;
  AnimationManager &animationManager_;
};

#endif // WEB_SERVER_MANAGER_HPP
