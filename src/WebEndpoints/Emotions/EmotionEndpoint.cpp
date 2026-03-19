#include "WebEndpoints/Emotions/EmotionEndpoint.hpp"

#include <ArduinoJson.h>

EmotionEndpoint::EmotionEndpoint(EmotionState &emotionState,
                                 EarController &earController)
    : emotionState_(emotionState), earController_(earController)
{
}

void EmotionEndpoint::registerEndpoint(AsyncWebServer &server)
{
  server.on("/emotion", HTTP_GET, [this](AsyncWebServerRequest *request)
            { handleGet(request); });
  server.on("/emotion", HTTP_POST, [this](AsyncWebServerRequest *request)
            { handlePost(request); });
  server.on("/emotion", HTTP_PUT, [this](AsyncWebServerRequest *request)
            { handlePut(request); });
  server.on("/emotion", HTTP_DELETE, [this](AsyncWebServerRequest *request)
            { handleDelete(request); });
}

bool EmotionEndpoint::parseEmotionDefinitionJson(const String &jsonPayload,
                                                 EmotionDefinition &emotion,
                                                 String &error) const
{
  JsonDocument document;
  const DeserializationError parseError =
      deserializeJson(document, jsonPayload);
  if (parseError)
  {
    error = "Invalid JSON payload.";
    return false;
  }

  JsonVariant root = document.as<JsonVariant>();
  if (!root.is<JsonObject>())
  {
    error = "Payload must be an object.";
    return false;
  }

  return emotion.deserialize(root.as<JsonObject>(), error);
}

void EmotionEndpoint::applyEmotionEarColor(const EmotionDefinition *emotion)
{
  if (emotion == nullptr)
  {
    return;
  }

  Ear &ear = earController_.getEar();
  if (emotion->earColorMode == ColorMode::Gradient)
  {
    Serial.println(F("[D] Setting ear gradient"));
    ear.setGradient(emotion->earGradient);
    return;
  }

  Serial.println(F("[D] Setting ear color"));
  ear.setColor(emotion->earColor);
}

bool EmotionEndpoint::hasEmotionUpdatePayload(AsyncWebServerRequest *request) const
{
  return request->hasParam("plain", true);
}

void EmotionEndpoint::handleGet(AsyncWebServerRequest *request)
{
  const auto *emotion = emotionState_.getCurrentEmotionDefinition();
  sendEmotionJson(request, *emotion);
}

void EmotionEndpoint::handlePost(AsyncWebServerRequest *request)
{
  if (!hasEmotionUpdatePayload(request))
  {
    request->send(400, "text/plain", F("JSON payload is required."));
    return;
  }

  EmotionDefinition emotion;
  String error;
  if (!parseEmotionDefinitionJson(request->getParam("plain", true)->value(),
                                  emotion, error))
  {
    request->send(400, "text/plain", error);
    return;
  }

  if (!emotionState_.upsertEmotionDefinition(emotion, false))
  {
    request->send(409, "text/plain", F("Emotion already exists."));
    return;
  }

  request->send(201, "text/plain", F("Emotion created."));
}

void EmotionEndpoint::handlePut(AsyncWebServerRequest *request)
{
  if (request->hasParam("name", true))
  {
    handleSetCurrentEmotion(request);
    return;
  }

  if (!hasEmotionUpdatePayload(request))
  {
    request->send(400, "text/plain",
                  F("JSON payload is required for emotion update."));
    return;
  }

  handleUpdateEmotionDefinition(request);
}

void EmotionEndpoint::handleDelete(AsyncWebServerRequest *request)
{
  if (!request->hasParam("name"))
  {
    request->send(400, "text/plain", F("Parameter 'name' is required."));
    return;
  }

  const String name = request->getParam("name")->value();
  if (!emotionState_.removeEmotionDefinitionByName(name))
  {
    request->send(404, "text/plain", F("Emotion not found."));
    return;
  }

  request->send(200, "text/plain", F("Emotion removed."));
}

void EmotionEndpoint::handleSetCurrentEmotion(AsyncWebServerRequest *request)
{
  emotionState_.setCurrentEmotion(request->getParam("name", true)->value());
  applyEmotionEarColor(emotionState_.getCurrentEmotionDefinition());
  request->send(200, "text/plain", F("Emotion changed."));
}

void EmotionEndpoint::handleUpdateEmotionDefinition(
    AsyncWebServerRequest *request)
{
  EmotionDefinition emotion;
  String error;
  if (!parseEmotionDefinitionJson(request->getParam("plain", true)->value(),
                                  emotion, error))
  {
    request->send(400, "text/plain", error);
    return;
  }

  if (emotionState_.getEmotionDefinitionByName(emotion.name) == nullptr &&
      emotionState_.getEmotionDefinitionByPath(emotion.path) == nullptr)
  {
    request->send(404, "text/plain", F("Emotion does not exist."));
    return;
  }

  emotionState_.upsertEmotionDefinition(emotion, true);

  if (emotionState_.getCurrentEmotion() == emotion.path ||
      (emotionState_.getCurrentEmotionDefinition() != nullptr &&
       emotionState_.getCurrentEmotionDefinition()->name == emotion.name))
  {
    applyEmotionEarColor(emotionState_.getCurrentEmotionDefinition());
  }

  request->send(200, "text/plain", F("Emotion updated."));
}

void EmotionEndpoint::sendEmotionJson(AsyncWebServerRequest *request,
                                      const EmotionDefinition &emotion) const
{
  JsonDocument document;
  JsonObject object = document.to<JsonObject>();
  emotion.serialize(object);

  String json;
  serializeJson(document, json);
  request->send(200, "application/json", json);
}
