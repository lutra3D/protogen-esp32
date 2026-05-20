#ifndef WEB_ENDPOINTS_SYSTEM_SYSTEM_POWER_ENDPOINT_HPP
#define WEB_ENDPOINTS_SYSTEM_SYSTEM_POWER_ENDPOINT_HPP

#include <ESPAsyncWebServer.h>

#include "SystemPowerController.hpp"

class SystemPowerEndpoint {
public:
  explicit SystemPowerEndpoint(SystemPowerController &systemPowerController);

  void registerEndpoint(AsyncWebServer &server);

private:
  void handleGet(AsyncWebServerRequest *request);

  SystemPowerController &systemPowerController_;
};

#endif // WEB_ENDPOINTS_SYSTEM_SYSTEM_POWER_ENDPOINT_HPP
