#include "WebEndpoints/Capabilities/CapabilitiesEndpoint.hpp"

#include <ArduinoJson.h>

CapabilitiesEndpoint::CapabilitiesEndpoint(CapabilityManager &capabilityManager)
    : capabilityManager_(capabilityManager)
{
}

void CapabilitiesEndpoint::registerEndpoint(AsyncWebServer &server)
{
  server.on("/capabilities", HTTP_GET, [this](AsyncWebServerRequest *request)
            { handleGet(request); });
}

void CapabilitiesEndpoint::handleGet(AsyncWebServerRequest *request)
{
  const auto capabilities = capabilityManager_.getAvailableCapabilities();

  JsonDocument document;
  JsonArray capabilitiesArray = document["capabilities"].to<JsonArray>();

  for (const auto &capability : capabilities)
  {
    capabilitiesArray.add(capability);
  }

  String json;
  serializeJson(document, json);

  request->send(200, "application/json", json);
}
