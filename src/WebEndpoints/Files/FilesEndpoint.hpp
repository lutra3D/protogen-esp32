#ifndef WEB_ENDPOINTS_FILES_FILES_ENDPOINT_HPP
#define WEB_ENDPOINTS_FILES_FILES_ENDPOINT_HPP

#include <ESPAsyncWebServer.h>

#include "AnimationManager.hpp"

class FilesEndpoint {
public:
  explicit FilesEndpoint(AnimationManager &animationManager);

  void registerEndpoint(AsyncWebServer &server);

private:
  void handleGet(AsyncWebServerRequest *request);

  AnimationManager &animationManager_;
};

#endif // WEB_ENDPOINTS_FILES_FILES_ENDPOINT_HPP
