#include "WebEndpoints/System/NotFoundEndpoint.hpp"

void NotFoundEndpoint::registerEndpoint(AsyncWebServer &server)
{
  server.onNotFound([this](AsyncWebServerRequest *request)
                    { handleNotFound(request); });
}

void NotFoundEndpoint::handleNotFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}
