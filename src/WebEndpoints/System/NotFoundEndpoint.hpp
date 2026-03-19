#ifndef WEB_ENDPOINTS_SYSTEM_NOT_FOUND_ENDPOINT_HPP
#define WEB_ENDPOINTS_SYSTEM_NOT_FOUND_ENDPOINT_HPP

#include <ESPAsyncWebServer.h>

class NotFoundEndpoint {
public:
  void registerEndpoint(AsyncWebServer &server);

private:
  void handleNotFound(AsyncWebServerRequest *request);
};

#endif // WEB_ENDPOINTS_SYSTEM_NOT_FOUND_ENDPOINT_HPP
