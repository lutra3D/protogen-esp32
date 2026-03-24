#ifndef WEB_ENDPOINTS_DEVICES_EARS_ENDPOINT_HPP
#define WEB_ENDPOINTS_DEVICES_EARS_ENDPOINT_HPP

#include <ESPAsyncWebServer.h>

#include "EarController.hpp"

class EarsEndpoint {
public:
  explicit EarsEndpoint(EarController &earController);

  void registerEndpoint(AsyncWebServer &server);

private:
  void handleGet(AsyncWebServerRequest *request);
  void handlePut(AsyncWebServerRequest *request);
  bool updateBrightness(AsyncWebServerRequest *request, Ear &ear,
                        bool &updated);

  EarController &earController_;
};

#endif // WEB_ENDPOINTS_DEVICES_EARS_ENDPOINT_HPP
