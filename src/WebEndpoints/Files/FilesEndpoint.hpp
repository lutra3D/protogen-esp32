#ifndef WEB_ENDPOINTS_FILES_FILES_ENDPOINT_HPP
#define WEB_ENDPOINTS_FILES_FILES_ENDPOINT_HPP

#include <ESPAsyncWebServer.h>

#include "FileManager.hpp"

class FilesEndpoint {
public:
  explicit FilesEndpoint(FileManager &fileManager);

  void registerEndpoint(AsyncWebServer &server);

private:
  void handleGet(AsyncWebServerRequest *request);
  void handleGetInfo(AsyncWebServerRequest *request);

  FileManager &fileManager_;
};

#endif // WEB_ENDPOINTS_FILES_FILES_ENDPOINT_HPP
