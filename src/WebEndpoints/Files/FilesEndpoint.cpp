#include "WebEndpoints/Files/FilesEndpoint.hpp"

#include <ArduinoJson.h>

FilesEndpoint::FilesEndpoint(AnimationManager &animationManager)
    : animationManager_(animationManager)
{
}

void FilesEndpoint::registerEndpoint(AsyncWebServer &server)
{
  server.on("/files", HTTP_GET, [this](AsyncWebServerRequest *request)
            { handleGet(request); });
}

void FilesEndpoint::handleGet(AsyncWebServerRequest *request)
{
  const auto emotionFiles = animationManager_.getEmotions();

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
