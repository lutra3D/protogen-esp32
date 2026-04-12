#ifndef WEB_ENDPOINTS_FILES_FILE_ENDPOINT_HPP
#define WEB_ENDPOINTS_FILES_FILE_ENDPOINT_HPP

#include <ESPAsyncWebServer.h>

#include <Arduino.h>

#include "FileManager.hpp"

class FileEndpoint {
public:
  explicit FileEndpoint(FileManager &fileManager);

  void registerEndpoint(AsyncWebServer &server);

private:
  struct UploadContext {
    String targetPath;
    String tempPath;
    bool overwrite = false;
    bool error = false;
    int statusCode = 500;
    String message;
  };

  void handleGet(AsyncWebServerRequest *request);
  void handleUploadComplete(AsyncWebServerRequest *request);
  void handleUploadChunk(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
  void handleDelete(AsyncWebServerRequest *request);

  void initializeUploadContext(AsyncWebServerRequest *request, String filename, size_t index);
  void writeUploadChunk(AsyncWebServerRequest *request, size_t index, size_t len, uint8_t *data);
  void finalizeUpload(AsyncWebServerRequest *request);

  bool resolveAndValidateFilePath(AsyncWebServerRequest *request, String &resolvedPath, bool animationsOnly) const;
  bool normalizeFilePath(const String &input, String &normalizedPath, bool animationsOnly) const;
  void sanitizeFilename(String &filename) const;
  UploadContext *getUploadContext(AsyncWebServerRequest *request) const;

  FileManager &fileManager_;
};

#endif // WEB_ENDPOINTS_FILES_FILE_ENDPOINT_HPP
