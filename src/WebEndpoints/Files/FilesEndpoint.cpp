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
  server.on("/files/info", HTTP_GET, [this](AsyncWebServerRequest *request)
            { handleGetInfo(request); });
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

void FilesEndpoint::handleGetInfo(AsyncWebServerRequest *request)
{
  const size_t totalSizeBytes = fileManager_.totalBytes();
  const size_t usedSizeBytes = fileManager_.usedBytes();

  float usedPercentage = 0.0F;
  if (totalSizeBytes > 0U)
  {
    usedPercentage = (static_cast<float>(usedSizeBytes) * 100.0F) /
                     static_cast<float>(totalSizeBytes);
  }

  JsonDocument document;
  JsonObject info = document.to<JsonObject>();
  info["totalSizeBytes"] = totalSizeBytes;
  info["usedSizeBytes"] = usedSizeBytes;
  info["usedPercentage"] = usedPercentage;

  String json;
  serializeJson(document, json);

  request->send(200, "application/json", json);
}
