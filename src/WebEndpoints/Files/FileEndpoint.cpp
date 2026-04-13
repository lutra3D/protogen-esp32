#include "WebEndpoints/Files/FileEndpoint.hpp"

#include <SPIFFS.h>

#include <FS.h>

FileEndpoint::FileEndpoint(FileManager &fileManager, bool allowAllFileChanges)
    : fileManager_(fileManager), allowAllFileChanges_(allowAllFileChanges)
{
}

void FileEndpoint::registerEndpoint(AsyncWebServer &server)
{
  server.on("/file", HTTP_GET, [this](AsyncWebServerRequest *request)
            { handleGet(request); });

  server.on(
      "/file", HTTP_POST,
      [this](AsyncWebServerRequest *request)
      {
        handleUploadComplete(request);
      },
      [this](AsyncWebServerRequest *request, String filename, size_t index,
             uint8_t *data, size_t len, bool final)
      {
        handleUploadChunk(request, filename, index, data, len, final);
      });

  server.on(
      "/file", HTTP_PUT,
      [this](AsyncWebServerRequest *request)
      {
        handleUploadComplete(request);
      },
      [this](AsyncWebServerRequest *request, String filename, size_t index,
             uint8_t *data, size_t len, bool final)
      {
        handleUploadChunk(request, filename, index, data, len, final);
      });

  server.on("/file", HTTP_DELETE, [this](AsyncWebServerRequest *request)
            { handleDelete(request); });
}

void FileEndpoint::handleGet(AsyncWebServerRequest *request)
{
  String filePath;
  if (!resolveAndValidateFilePath(request, filePath, false))
  {
    request->send(400, "text/plain",
                  F("Missing or invalid 'file' parameter (use /anims/* path)."));
    return;
  }

  if (!fileManager_.exists(filePath))
  {
    request->send(404, "text/plain", F("File not found."));
    return;
  }

  request->send(SPIFFS, filePath, String(), true);
}

void FileEndpoint::handleUploadComplete(AsyncWebServerRequest *request)
{
  auto *context = getUploadContext(request);
  if (context == nullptr)
  {
    request->send(400, "text/plain", F("No file uploaded."));
    return;
  }

  const int statusCode = context->error ? context->statusCode : 200;
  String message = context->message;
  if (!message.length())
  {
    message = F("File was saved.");
  }

  request->send(statusCode, "text/plain", message);

  delete context;
  request->_tempObject = nullptr;
}

void FileEndpoint::handleUploadChunk(AsyncWebServerRequest *request,
                                     String filename, size_t index,
                                     uint8_t *data, size_t len, bool final)
{
  initializeUploadContext(request, filename, index);

  auto *context = getUploadContext(request);
  if (context == nullptr)
  {
    return;
  }

  if (!context->error)
  {
    writeUploadChunk(request, index, len, data);
  }

  if (final)
  {
    finalizeUpload(request);
  }
}

void FileEndpoint::handleDelete(AsyncWebServerRequest *request)
{
  String filePath;
  if (!resolveAndValidateFilePath(request, filePath, true))
  {
    request->send(400, "text/plain", F("Missing or invalid 'file' parameter (use /anims/* path)."));
    return;
  }

  if (!fileManager_.exists(filePath))
  {
    request->send(404, "text/plain", F("File not found."));
    return;
  }

  if (!fileManager_.removeFile(filePath))
  {
    request->send(500, "text/plain", F("Failed to delete file."));
    return;
  }

  request->send(200, "text/plain", F("File was deleted."));
}

void FileEndpoint::initializeUploadContext(AsyncWebServerRequest *request,
                                           String filename, size_t index)
{
  if (index != 0)
  {
    return;
  }

  auto *context = getUploadContext(request);
  if (context != nullptr)
  {
    delete context;
    context = nullptr;
  }

  context = new UploadContext();
  request->_tempObject = context;

  if (request->hasParam("file", true))
  {
    filename = request->getParam("file", true)->value();
  }
  else if (request->hasParam("file"))
  {
    filename = request->getParam("file")->value();
  }

  filename.trim();
  if (filename.isEmpty())
  {
    context->error = true;
    context->statusCode = 400;
    context->message = F("File name is required.");
    return;
  }

  if (!normalizeFilePath(filename, context->targetPath, true))
  {
    context->error = true;
    context->statusCode = 400;
    context->message = F("Invalid file path. Use /anims/* path.");
    return;
  }

  const bool isPut = request->method() == HTTP_PUT;
  context->overwrite = isPut;

  const bool exists = fileManager_.exists(context->targetPath);
  if (isPut && !exists)
  {
    context->error = true;
    context->statusCode = 404;
    context->message = F("Cannot update missing file.");
    return;
  }

  if (!isPut && exists)
  {
    context->error = true;
    context->statusCode = 409;
    context->message = F("File already exists.");
    return;
  }

  context->tempPath = context->targetPath + F(".tmp");
  if (fileManager_.exists(context->tempPath) &&
      !fileManager_.removeFile(context->tempPath))
  {
    context->error = true;
    context->statusCode = 500;
    context->message = F("Failed to prepare temporary file.");
  }
}

void FileEndpoint::writeUploadChunk(AsyncWebServerRequest *request, size_t index,
                                    size_t len, uint8_t *data)
{
  auto *context = getUploadContext(request);
  if (context == nullptr || len == 0)
  {
    return;
  }

  const bool append = index > 0;
  if (!fileManager_.writeFile(context->tempPath, data, len, append))
  {
    context->error = true;
    context->statusCode = 500;
    context->message = F("Failed to write uploaded data.");
  }
}

void FileEndpoint::finalizeUpload(AsyncWebServerRequest *request)
{
  auto *context = getUploadContext(request);
  if (context == nullptr)
  {
    return;
  }

  if (context->error)
  {
    if (fileManager_.exists(context->tempPath))
    {
      fileManager_.removeFile(context->tempPath);
    }
    return;
  }

  if (context->overwrite && fileManager_.exists(context->targetPath) &&
      !fileManager_.removeFile(context->targetPath))
  {
    context->error = true;
    context->statusCode = 500;
    context->message = F("Failed to replace existing file.");
    fileManager_.removeFile(context->tempPath);
    return;
  }

  if (!fileManager_.renameFile(context->tempPath, context->targetPath))
  {
    context->error = true;
    context->statusCode = 500;
    context->message = F("Failed to save file.");
    fileManager_.removeFile(context->tempPath);
    return;
  }

  context->message = context->overwrite ? F("File was updated.") : F("File was created.");
}

bool FileEndpoint::resolveAndValidateFilePath(AsyncWebServerRequest *request, String &resolvedPath, bool animationsOnly) const
{
  if (request->hasParam("file"))
  {
    return normalizeFilePath(request->getParam("file")->value(), resolvedPath, animationsOnly);
  }

  if (request->hasParam("path"))
  {
    return normalizeFilePath(request->getParam("path")->value(), resolvedPath, animationsOnly);
  }

  return false;
}

bool FileEndpoint::normalizeFilePath(const String &input,  String &normalizedPath, bool animationsOnly) const
{
  normalizedPath = input;
  normalizedPath.trim();

  if (normalizedPath.isEmpty())
  {
    return false;
  }

  sanitizeFilename(normalizedPath);

  if(!animationsOnly || allowAllFileChanges_){
    return true;
  }

  if (normalizedPath.startsWith("/anims/"))
  {
    return normalizedPath.indexOf("..") < 0;
  }

  if (normalizedPath.startsWith("/"))
  {
    return false;
  }

  normalizedPath = "/anims/" + normalizedPath;
  return normalizedPath.indexOf("..") < 0;
}

void FileEndpoint::sanitizeFilename(String &filename) const
{
  filename.replace('\\', '/');
}

FileEndpoint::UploadContext *FileEndpoint::getUploadContext(
    AsyncWebServerRequest *request) const
{
  return static_cast<UploadContext *>(request->_tempObject);
}
