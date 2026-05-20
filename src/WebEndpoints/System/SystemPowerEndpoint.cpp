#include "WebEndpoints/System/SystemPowerEndpoint.hpp"

SystemPowerEndpoint::SystemPowerEndpoint(SystemPowerController &systemPowerController)
    : systemPowerController_(systemPowerController) {}

void SystemPowerEndpoint::registerEndpoint(AsyncWebServer &server) {
  server.on("/system-power", HTTP_GET, [this](AsyncWebServerRequest *request) { handleGet(request); });
}

void SystemPowerEndpoint::handleGet(AsyncWebServerRequest *request) {
  if (!systemPowerController_.isEnabled()) {
    request->send(200, "text/plain", F("System power sensor is disabled"));
    return;
  }

  request->send(200, "text/plain", systemPowerController_.readPowerInfo());
}
