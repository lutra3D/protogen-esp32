#ifndef WEB_ENDPOINTS_CAPABILITIES_CAPABILITIES_ENDPOINT_HPP
#define WEB_ENDPOINTS_CAPABILITIES_CAPABILITIES_ENDPOINT_HPP

#include <ESPAsyncWebServer.h>

#include "Capabilities/CapabilityManager.hpp"

class CapabilitiesEndpoint
{
public:
  explicit CapabilitiesEndpoint(CapabilityManager &capabilityManager);

  void registerEndpoint(AsyncWebServer &server);

private:
  void handleGet(AsyncWebServerRequest *request);

  CapabilityManager &capabilityManager_;
};

#endif // WEB_ENDPOINTS_CAPABILITIES_CAPABILITIES_ENDPOINT_HPP
