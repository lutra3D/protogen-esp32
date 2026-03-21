#ifndef WEB_ENDPOINTS_EMOTIONS_EMOTIONS_ENDPOINT_HPP
#define WEB_ENDPOINTS_EMOTIONS_EMOTIONS_ENDPOINT_HPP

#include <ESPAsyncWebServer.h>

#include "EmotionState.hpp"

class EmotionsEndpoint {
public:
  explicit EmotionsEndpoint(EmotionState &emotionState);

  void registerEndpoint(AsyncWebServer &server);

private:
  void handleGet(AsyncWebServerRequest *request);

  EmotionState &emotionState_;
};

#endif // WEB_ENDPOINTS_EMOTIONS_EMOTIONS_ENDPOINT_HPP
