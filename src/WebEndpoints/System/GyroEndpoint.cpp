#include "WebEndpoints/System/GyroEndpoint.hpp"

GyroEndpoint::GyroEndpoint(TiltController &tiltController)
    : tiltController_(tiltController)
{
}

void GyroEndpoint::registerEndpoint(AsyncWebServer &server)
{
  server.on("/gyro", HTTP_GET, [this](AsyncWebServerRequest *request)
            { handleGet(request); });
}

void GyroEndpoint::handleGet(AsyncWebServerRequest *request)
{
  if (!tiltController_.isEnabled())
  {
    request->send(200, "text/plain", F("Tilt is disabled"));
    return;
  }

  request->send(200, "text/plain", tiltController_.readAcceleration());
}
