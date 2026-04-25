#ifndef WEB_ENDPOINTS_SYSTEM_STATIC_CONTENT_ENDPOINT_HPP
#define WEB_ENDPOINTS_SYSTEM_STATIC_CONTENT_ENDPOINT_HPP

#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

class StaticContentEndpoint {
public:
  void registerEndpoint(AsyncWebServer &server);

private:
  void registerStaticContent(AsyncWebServer &server);
};

#endif // WEB_ENDPOINTS_SYSTEM_STATIC_CONTENT_ENDPOINT_HPP
