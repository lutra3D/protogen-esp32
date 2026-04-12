#ifndef WEB_ENDPOINTS_DEVICES_EARS_ENDPOINT_HPP
#define WEB_ENDPOINTS_DEVICES_EARS_ENDPOINT_HPP

#include <ESPAsyncWebServer.h>
#include <functional>

#include "EarController.hpp"

class EarsEndpoint {
public:
  EarsEndpoint(EarController &earController,
               std::function<void()> onSettingsChanged);

  void registerEndpoint(AsyncWebServer &server);

private:
  void handleGet(AsyncWebServerRequest *request);
  void handlePut(AsyncWebServerRequest *request);
  bool updateBrightness(AsyncWebServerRequest *request, Ear &ear,
                        bool &updated);

  EarController &earController_;
  std::function<void()> onSettingsChanged_;
};

#endif // WEB_ENDPOINTS_DEVICES_EARS_ENDPOINT_HPP
