#ifndef WEB_ENDPOINTS_SYSTEM_HEAP_ENDPOINT_HPP
#define WEB_ENDPOINTS_SYSTEM_HEAP_ENDPOINT_HPP

#include <ESPAsyncWebServer.h>

#include <Arduino.h>

class HeapEndpoint {
public:
  void registerEndpoint(AsyncWebServer &server);

private:
  void handleGet(AsyncWebServerRequest *request);
};

#endif // WEB_ENDPOINTS_SYSTEM_HEAP_ENDPOINT_HPP
