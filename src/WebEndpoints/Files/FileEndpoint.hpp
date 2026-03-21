#ifndef WEB_ENDPOINTS_FILES_FILE_ENDPOINT_HPP
#define WEB_ENDPOINTS_FILES_FILE_ENDPOINT_HPP

#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

#include <Arduino.h>

class FileEndpoint {
public:
  void registerEndpoint(AsyncWebServer &server);

private:
  struct UploadContext {
    String targetPath;
    String tempPath;
    bool error = false;
    String message;
  };

  void handleUploadComplete(AsyncWebServerRequest *request);
  void handleUploadChunk(AsyncWebServerRequest *request, String filename,
                         size_t index, uint8_t *data, size_t len,
                         bool final);
  void handleDelete(AsyncWebServerRequest *request);
  void initializeUploadContext(AsyncWebServerRequest *request, String &filename,
                               size_t index);
  void writeUploadChunk(AsyncWebServerRequest *request, size_t len,
                        uint8_t *data);
  void finalizeUpload(AsyncWebServerRequest *request);
  UploadContext *getUploadContext(AsyncWebServerRequest *request) const;
  void cleanupUploadFile(AsyncWebServerRequest *request) const;
  void sanitizeFilename(String &filename) const;
};

#endif // WEB_ENDPOINTS_FILES_FILE_ENDPOINT_HPP
