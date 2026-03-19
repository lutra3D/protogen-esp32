#ifndef WEB_ENDPOINTS_EMOTIONS_EMOTION_ENDPOINT_HPP
#define WEB_ENDPOINTS_EMOTIONS_EMOTION_ENDPOINT_HPP

#include <ESPAsyncWebServer.h>

#include "EarController.hpp"
#include "EmotionState.hpp"

class EmotionEndpoint {
public:
  EmotionEndpoint(EmotionState &emotionState, EarController &earController);

  void registerEndpoint(AsyncWebServer &server);

private:
  bool parseEmotionDefinitionJson(const String &jsonPayload,
                                  EmotionDefinition &emotion,
                                  String &error) const;
  void applyEmotionEarColor(const EmotionDefinition *emotion);
  bool hasEmotionUpdatePayload(AsyncWebServerRequest *request) const;
  void handleGet(AsyncWebServerRequest *request);
  void handlePost(AsyncWebServerRequest *request);
  void handlePut(AsyncWebServerRequest *request);
  void handleDelete(AsyncWebServerRequest *request);
  void handleSetCurrentEmotion(AsyncWebServerRequest *request);
  void handleUpdateEmotionDefinition(AsyncWebServerRequest *request);
  void sendEmotionJson(AsyncWebServerRequest *request,
                       const EmotionDefinition &emotion) const;

  EmotionState &emotionState_;
  EarController &earController_;
};

#endif // WEB_ENDPOINTS_EMOTIONS_EMOTION_ENDPOINT_HPP
