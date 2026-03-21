#include "WebEndpoints/Files/FileEndpoint.hpp"

#include <FS.h>

void FileEndpoint::registerEndpoint(AsyncWebServer &server)
{
  server.on(
      "/file", HTTP_POST,
      [this](AsyncWebServerRequest *request)
      {
        handleUploadComplete(request);
      },
      [this](AsyncWebServerRequest *request, String filename, size_t index,
             uint8_t *data, size_t len, bool final)
      {
        Serial.println(F("[I] File upload callback called"));
        handleUploadChunk(request, filename, index, data, len, final);
      });

  server.on("/file", HTTP_DELETE, [this](AsyncWebServerRequest *request)
            { handleDelete(request); });
}

void FileEndpoint::handleUploadComplete(AsyncWebServerRequest *request)
{
  auto *context = getUploadContext(request);
  if (context == nullptr)
  {
    request->send(400, "text/plain", F("No file uploaded."));
    return;
  }

  String message = context->message;
  if (!message.length())
  {
    message = F("File was saved.");
  }

  if (context->error)
  {
    request->send(400, "text/plain", message);
  }
  else
  {
    request->send(200, "text/plain", message);
  }

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

  if (context->error)
  {
    if (final)
    {
      cleanupUploadFile(request);
    }
    return;
  }

  writeUploadChunk(request, len, data);

  if (!final)
  {
    return;
  }

  finalizeUpload(request);
}

void FileEndpoint::handleDelete(AsyncWebServerRequest *request)
{
  if (!request->hasParam("file"))
  {
    request->send(400, "text/plain", F("Parameter 'file' not present!"));
    return;
  }

  const String filePath = request->getParam("file")->value();
  Serial.println("[I] Deleting " + filePath);

  if (!SPIFFS.exists(filePath))
  {
    request->send(400, "text/plain", F("File does not exist!"));
    return;
  }

  if (!filePath.startsWith("/anims/"))
  {
    request->send(400, "text/plain",
                  F("Cannot delete the file, file is not an animation!"));
    return;
  }

  SPIFFS.remove(filePath);
  request->send(200, "text/plain", F("File was deleted."));
}

void FileEndpoint::initializeUploadContext(AsyncWebServerRequest *request,
                                           String &filename, size_t index)
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

  filename.trim();
  if (filename.isEmpty())
  {
    context->error = true;
    context->message = F("File name is required.");
    return;
  }

  sanitizeFilename(filename);
  context->targetPath = "/anims/" + filename;
  context->tempPath = context->targetPath + F(".tmp");

  if (SPIFFS.exists(context->tempPath))
  {
    SPIFFS.remove(context->tempPath);
  }

  request->_tempFile = SPIFFS.open(context->tempPath, FILE_WRITE);
  if (!request->_tempFile)
  {
    context->error = true;
    context->message = F("Error opening temporary file for writing!");
  }
}

void FileEndpoint::writeUploadChunk(AsyncWebServerRequest *request, size_t len,
                                    uint8_t *data)
{
  auto *context = getUploadContext(request);
  if (context == nullptr || len == 0 || !request->_tempFile)
  {
    return;
  }

  const size_t written = request->_tempFile.write(data, len);
  if (written == len || context->error)
  {
    return;
  }

  context->error = true;
  context->message = F("Failed to write uploaded data.");
  cleanupUploadFile(request);
  if (SPIFFS.exists(context->tempPath))
  {
    SPIFFS.remove(context->tempPath);
  }
}

void FileEndpoint::finalizeUpload(AsyncWebServerRequest *request)
{
  auto *context = getUploadContext(request);
  if (context == nullptr)
  {
    return;
  }

  cleanupUploadFile(request);

  if (context->error)
  {
    if (SPIFFS.exists(context->tempPath))
    {
      SPIFFS.remove(context->tempPath);
    }
    return;
  }

  if (SPIFFS.exists(context->targetPath))
  {
    SPIFFS.remove(context->targetPath);
  }

  if (!SPIFFS.rename(context->tempPath, context->targetPath))
  {
    context->error = true;
    context->message = F("Failed to save file.");
    if (SPIFFS.exists(context->tempPath))
    {
      SPIFFS.remove(context->tempPath);
    }
    return;
  }

  context->message = F("File was saved.");
}

FileEndpoint::UploadContext *FileEndpoint::getUploadContext(
    AsyncWebServerRequest *request) const
{
  return static_cast<UploadContext *>(request->_tempObject);
}

void FileEndpoint::cleanupUploadFile(AsyncWebServerRequest *request) const
{
  if (request->_tempFile)
  {
    request->_tempFile.close();
    request->_tempFile = File();
  }
}

void FileEndpoint::sanitizeFilename(String &filename) const
{
  for (size_t i = 0; i < filename.length(); ++i)
  {
    const char currentChar = filename.charAt(i);
    if (currentChar == '/' || currentChar == '\\')
    {
      filename.setCharAt(i, '_');
    }
  }
}
