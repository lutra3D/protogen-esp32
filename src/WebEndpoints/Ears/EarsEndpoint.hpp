#ifndef WEB_ENDPOINTS_BRIGHTNESS_ENDPOINT_HPP
#define WEB_ENDPOINTS_BRIGHTNESS_ENDPOINT_HPP

#include <ESPAsyncWebServer.h>
#include <functional>

#include "LedBrightnessController.hpp"

class EarsEndpoint {
public:
  EarsEndpoint(LedBrightnessController &brightnessController,
               std::function<void()> onSettingsChanged);

  void registerEndpoint(AsyncWebServer &server);

private:
  void handleGet(AsyncWebServerRequest *request);
  void handlePut(AsyncWebServerRequest *request);

  LedBrightnessController &brightnessController_;
  std::function<void()> onSettingsChanged_;
};

#endif
