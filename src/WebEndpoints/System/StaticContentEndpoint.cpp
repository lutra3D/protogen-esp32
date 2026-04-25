#include "WebEndpoints/System/StaticContentEndpoint.hpp"

void StaticContentEndpoint::registerEndpoint(AsyncWebServer &server)
{
  registerStaticContent(server);
}

void StaticContentEndpoint::registerStaticContent(AsyncWebServer &server)
{
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
}
