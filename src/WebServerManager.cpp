#include "WebServerManager.hpp"

WebServerManager::WebServerManager(
    EmotionState &emotionState,
    FanController &fanController,
    EarController &earController,
    LedBrightnessController &brightnessController,
    TiltController &tiltController,
    SystemPowerController &systemPowerController,
    FileManager &fileManager,
    CapabilityManager &capabilityManager,
    std::function<void()> onSettingsChanged,
    bool allowAllFileChanges)
    : server_(80),
      staticContentEndpoint_(),
      fileEndpoint_(fileManager, allowAllFileChanges),
      filesEndpoint_(fileManager),
      emotionsEndpoint_(emotionState),
      emotionEndpoint_(emotionState, earController, onSettingsChanged),
      heapEndpoint_(),
      fanEndpoint_(fanController, onSettingsChanged),
      earsEndpoint_(brightnessController, onSettingsChanged),
      gyroEndpoint_(tiltController),
      systemPowerEndpoint_(systemPowerController),
      capabilitiesEndpoint_(capabilityManager),
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
  systemPowerEndpoint_.registerEndpoint(server_);
  capabilitiesEndpoint_.registerEndpoint(server_);
  notFoundEndpoint_.registerEndpoint(server_);
}
