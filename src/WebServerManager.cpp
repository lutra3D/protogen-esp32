#include "WebServerManager.hpp"

WebServerManager::WebServerManager(
    EmotionState &emotionState,
    FanController &fanController,
    EarController &earController,
    TiltController &tiltController,
    AnimationManager &animationManager)
    : server_(80),
      staticContentEndpoint_(),
      fileEndpoint_(),
      filesEndpoint_(animationManager),
      emotionsEndpoint_(emotionState),
      emotionEndpoint_(emotionState, earController),
      heapEndpoint_(),
      fanEndpoint_(fanController),
      earsEndpoint_(earController),
      gyroEndpoint_(tiltController),
      notFoundEndpoint_()
{
}

void WebServerManager::begin(const char *ssid, const char *password)
{
  WiFi.softAP(ssid, password);

  registerRoutes();

  ElegantOTA.begin(&server_);
  server_.begin();
  ElegantOTA.setAutoReboot(true);
}

void WebServerManager::loop()
{
  ElegantOTA.loop();
}

void WebServerManager::registerRoutes()
{
  staticContentEndpoint_.registerEndpoint(server_);
  fileEndpoint_.registerEndpoint(server_);
  filesEndpoint_.registerEndpoint(server_);
  emotionsEndpoint_.registerEndpoint(server_);
  emotionEndpoint_.registerEndpoint(server_);
  heapEndpoint_.registerEndpoint(server_);
  fanEndpoint_.registerEndpoint(server_);
  earsEndpoint_.registerEndpoint(server_);
  gyroEndpoint_.registerEndpoint(server_);
  notFoundEndpoint_.registerEndpoint(server_);
}
