#ifndef WEB_ENDPOINTS_DEVICES_FAN_ENDPOINT_HPP
#define WEB_ENDPOINTS_DEVICES_FAN_ENDPOINT_HPP

#include <ESPAsyncWebServer.h>

#include "FanController.hpp"

class FanEndpoint {
public:
  explicit FanEndpoint(FanController &fanController);

  void registerEndpoint(AsyncWebServer &server);

private:
  void handleGet(AsyncWebServerRequest *request);
  void handlePut(AsyncWebServerRequest *request);

  FanController &fanController_;
};

#endif // WEB_ENDPOINTS_DEVICES_FAN_ENDPOINT_HPP
