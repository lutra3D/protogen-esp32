#include "WebEndpoints/System/HeapEndpoint.hpp"

void HeapEndpoint::registerEndpoint(AsyncWebServer &server)
{
  server.on("/heap", HTTP_GET, [this](AsyncWebServerRequest *request)
            { handleGet(request); });
}

void HeapEndpoint::handleGet(AsyncWebServerRequest *request)
{
  request->send(200, "text/plain", String(ESP.getFreeHeap()));
}
