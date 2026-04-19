#ifndef WEB_SERVER_MANAGER_HPP
#define WEB_SERVER_MANAGER_HPP

#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>
#include <WiFi.h>

#include <Arduino.h>
#include <functional>

#include "FileManager.hpp"
#include "EarController.hpp"
#include "EmotionState.hpp"
#include "FanController.hpp"
#include "TiltController.hpp"
#include "WebEndpoints/Ears/EarsEndpoint.hpp"
#include "WebEndpoints/Fan/FanEndpoint.hpp"
#include "WebEndpoints/Emotions/EmotionEndpoint.hpp"
#include "WebEndpoints/Emotions/EmotionsEndpoint.hpp"
#include "WebEndpoints/Files/FileEndpoint.hpp"
#include "WebEndpoints/Files/FilesEndpoint.hpp"
#include "WebEndpoints/System/GyroEndpoint.hpp"
#include "Capabilities/CapabilityManager.hpp"
#include "WebEndpoints/Capabilities/CapabilitiesEndpoint.hpp"
#include "WebEndpoints/System/HeapEndpoint.hpp"
#include "WebEndpoints/System/NotFoundEndpoint.hpp"
#include "WebEndpoints/System/StaticContentEndpoint.hpp"
class WebServerManager
{
public:
  WebServerManager(EmotionState &emotionState, FanController &fanController,
                   EarController &earController, TiltController &tiltController,
                   FileManager &fileManager,
                   CapabilityManager &capabilityManager,
                   std::function<void()> onSettingsChanged, bool allowAllFileChanges);

  void begin(const char *ssid, const char *password);
  void loop();

private:
  void registerRoutes();

  AsyncWebServer server_;
  StaticContentEndpoint staticContentEndpoint_;
  FileEndpoint fileEndpoint_;
  FilesEndpoint filesEndpoint_;
  EmotionsEndpoint emotionsEndpoint_;
  EmotionEndpoint emotionEndpoint_;
  HeapEndpoint heapEndpoint_;
  FanEndpoint fanEndpoint_;
  EarsEndpoint earsEndpoint_;
  GyroEndpoint gyroEndpoint_;
  CapabilitiesEndpoint capabilitiesEndpoint_;
  NotFoundEndpoint notFoundEndpoint_;
};

#endif // WEB_SERVER_MANAGER_HPP
