#include "WebEndpoints/Files/FilesEndpoint.hpp"

#include <ArduinoJson.h>

FilesEndpoint::FilesEndpoint(FileManager &fileManager)
    : fileManager_(fileManager)
{
}

void FilesEndpoint::registerEndpoint(AsyncWebServer &server)
{
  server.on("/files", HTTP_GET, [this](AsyncWebServerRequest *request)
            { handleGet(request); });
}

void FilesEndpoint::handleGet(AsyncWebServerRequest *request)
{
  const String filter = request->hasParam("filter")
                            ? request->getParam("filter")->value()
                            : String();
  const auto emotionFiles = fileManager_.getFiles(filter);

  JsonDocument document;
  JsonArray files = document.to<JsonArray>();

  for (const auto &file : emotionFiles)
  {
    JsonObject fileObject = files.add<JsonObject>();
    file.serialize(fileObject);
  }

  String json;
  serializeJson(document, json);

  request->send(200, "application/json", json);
}
