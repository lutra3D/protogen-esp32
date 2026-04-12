#ifndef WEB_ENDPOINTS_EMOTIONS_EMOTION_ENDPOINT_HPP
#define WEB_ENDPOINTS_EMOTIONS_EMOTION_ENDPOINT_HPP

#include <ESPAsyncWebServer.h>
#include <functional>

#include "Web/JsonEndpoint.hpp"
#include "EarController.hpp"
#include "EmotionState.hpp"

class EmotionEndpoint : public JsonEndpoint
{
public:
  EmotionEndpoint(EmotionState &emotionState, EarController &earController,
                  std::function<void()> onSettingsChanged);

  void registerEndpoint(AsyncWebServer &server);

private:
  bool parseEmotionDefinitionJson(const String &jsonPayload,  EmotionDefinition &emotion, String &error) const;
  void applyEmotionEarColor(const EmotionDefinition *emotion);
  void handleGet(AsyncWebServerRequest *request);
  Response handlePost(AsyncWebServerRequest *request, JsonDocument &doc);
  Response handlePut(AsyncWebServerRequest *request, JsonDocument &doc);
  void handleDelete(AsyncWebServerRequest *request);
  void handleSetCurrentEmotion(AsyncWebServerRequest *request);
  void sendEmotionJson(AsyncWebServerRequest *request, const EmotionDefinition &emotion) const;

  EmotionState &emotionState_;
  EarController &earController_;
  std::function<void()> onSettingsChanged_;
};

#endif // WEB_ENDPOINTS_EMOTIONS_EMOTION_ENDPOINT_HPP
