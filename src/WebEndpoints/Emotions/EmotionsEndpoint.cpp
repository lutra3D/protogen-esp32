#include "WebEndpoints/Emotions/EmotionsEndpoint.hpp"

#include <ArduinoJson.h>

EmotionsEndpoint::EmotionsEndpoint(EmotionState &emotionState)
    : emotionState_(emotionState)
{
}

void EmotionsEndpoint::registerEndpoint(AsyncWebServer &server)
{
  server.on("/emotions", HTTP_GET, [this](AsyncWebServerRequest *request)
            { handleGet(request); });
}

void EmotionsEndpoint::handleGet(AsyncWebServerRequest *request)
{
  JsonDocument document;
  JsonArray emotions = document.to<JsonArray>();
  for (const auto &emotion : emotionState_.getEmotionDefinitions())
  {
    JsonObject emotionObject = emotions.add<JsonObject>();
    emotion.serialize(emotionObject);
  }

  String json;
  serializeJson(document, json);
  request->send(200, "application/json", json);
}
