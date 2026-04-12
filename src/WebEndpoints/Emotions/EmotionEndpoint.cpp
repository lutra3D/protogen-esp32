#include "WebEndpoints/Emotions/EmotionEndpoint.hpp"

#include <ArduinoJson.h>

EmotionEndpoint::EmotionEndpoint(EmotionState &emotionState,
                                 EarController &earController,
                                 std::function<void()> onSettingsChanged)
    : emotionState_(emotionState),
      earController_(earController),
      onSettingsChanged_(onSettingsChanged)
{
}

void EmotionEndpoint::registerEndpoint(AsyncWebServer &server)
{
  server.on("/emotion", HTTP_GET, [this](AsyncWebServerRequest *request)  { handleGet(request); });
  server.on("/emotion/current", HTTP_PUT, [this](AsyncWebServerRequest *request)  { handleSetCurrentEmotion(request); });
  addJsonHandler(
    server, 
    HTTP_POST,
    "/emotion",
    [this](AsyncWebServerRequest *request, JsonDocument &doc) { 
      return handlePost(request, doc); 
    });
  addJsonHandler(
    server,
    HTTP_PUT, 
    "/emotion", 
    [this](AsyncWebServerRequest *request, JsonDocument &doc) 
    { 
      return handlePut(request, doc); 
    });
  server.on("/emotion", HTTP_DELETE, [this](AsyncWebServerRequest *request)  { handleDelete(request); });
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

void EmotionEndpoint::handleGet(AsyncWebServerRequest *request)
{
  const auto *emotion = emotionState_.getCurrentEmotionDefinition();
  sendEmotionJson(request, *emotion);
}

Response EmotionEndpoint::handlePost(AsyncWebServerRequest *request, JsonDocument &doc)
{
  if (!doc.is<JsonObject>())
  {
    return {F("JSON payload is required."), "text/plain", 400};
  }

  EmotionDefinition emotion;
  String error;

  if (!emotion.deserialize(doc.as<JsonObject>(), error))
  {
    return { error, "text/plain", 400};
  }

  if (!emotionState_.upsertEmotionDefinition(emotion, false))
  {
    return {F("Emotion already exists."), "text/plain", 409};
  }

  if (onSettingsChanged_)
  {
    onSettingsChanged_();
  }
  return {F("Emotion created."), "text/plain", 201};
}

Response EmotionEndpoint::handlePut(AsyncWebServerRequest *request, JsonDocument &doc)
{
  if (!doc.is<JsonObject>())
  {
    return {F("JSON payload is required."), "text/plain", 400};
  }

  EmotionDefinition emotion;
  String error;
  
  if (!emotion.deserialize(doc.as<JsonObject>(), error))
  {
    return { error, "text/plain", 400};
  }

  if (emotionState_.getEmotionDefinitionByName(emotion.name) == nullptr &&
      emotionState_.getEmotionDefinitionByPath(emotion.path) == nullptr)
  {
    return {F("Emotion does not exist."), "text/plain", 404};
  }

  emotionState_.upsertEmotionDefinition(emotion, true);

  if (emotionState_.getCurrentEmotion() == emotion.path ||
      (emotionState_.getCurrentEmotionDefinition() != nullptr &&
       emotionState_.getCurrentEmotionDefinition()->name == emotion.name))
  {
    applyEmotionEarColor(emotionState_.getCurrentEmotionDefinition());
  }

  if (onSettingsChanged_)
  {
    onSettingsChanged_();
  }
  return {F("Emotion updated."), "text/plain", 200};
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
  if (onSettingsChanged_)
  {
    onSettingsChanged_();
  }
  request->send(200, "text/plain", F("Emotion changed."));
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
