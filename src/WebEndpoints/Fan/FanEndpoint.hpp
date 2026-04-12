#ifndef WEB_ENDPOINTS_DEVICES_FAN_ENDPOINT_HPP
#define WEB_ENDPOINTS_DEVICES_FAN_ENDPOINT_HPP

#include <ESPAsyncWebServer.h>
#include <functional>

#include "FanController.hpp"

class FanEndpoint {
public:
  FanEndpoint(FanController &fanController, std::function<void()> onSettingsChanged);

  void registerEndpoint(AsyncWebServer &server);

private:
  void handleGet(AsyncWebServerRequest *request);
  void handlePut(AsyncWebServerRequest *request);

  FanController &fanController_;
  std::function<void()> onSettingsChanged_;
};

#endif // WEB_ENDPOINTS_DEVICES_FAN_ENDPOINT_HPP
