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
  const auto files = fileManager_.getFiles(filter);

  JsonDocument document;
  JsonArray jsonfiles = document.to<JsonArray>();

  for (const auto &file : files)
  {
    JsonObject jsonFile = jsonfiles.add<JsonObject>();
    file.serialize(jsonFile);
  }

  String json;
  serializeJson(document, json);

  request->send(200, "application/json", json);
}
