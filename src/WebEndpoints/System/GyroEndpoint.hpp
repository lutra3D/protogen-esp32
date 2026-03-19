#ifndef WEB_ENDPOINTS_SYSTEM_GYRO_ENDPOINT_HPP
#define WEB_ENDPOINTS_SYSTEM_GYRO_ENDPOINT_HPP

#include <ESPAsyncWebServer.h>

#include "TiltController.hpp"

class GyroEndpoint {
public:
  explicit GyroEndpoint(TiltController &tiltController);

  void registerEndpoint(AsyncWebServer &server);

private:
  void handleGet(AsyncWebServerRequest *request);

  TiltController &tiltController_;
};

#endif // WEB_ENDPOINTS_SYSTEM_GYRO_ENDPOINT_HPP
