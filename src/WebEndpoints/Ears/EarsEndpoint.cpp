#include "WebEndpoints/Ears/EarsEndpoint.hpp"

#include <ArduinoJson.h>

EarsEndpoint::EarsEndpoint(LedBrightnessController &brightnessController,
                           std::function<void()> onSettingsChanged)
    : brightnessController_(brightnessController),
      onSettingsChanged_(onSettingsChanged)
{
}

void EarsEndpoint::registerEndpoint(AsyncWebServer &server)
{
  server.on("/brightness", HTTP_GET, [this](AsyncWebServerRequest *request) { handleGet(request); });
  server.on("/brightness", HTTP_PUT, [this](AsyncWebServerRequest *request) { handlePut(request); });
}

void EarsEndpoint::handleGet(AsyncWebServerRequest *request)
{
  JsonDocument document;
  JsonObject object = document.to<JsonObject>();
  brightnessController_.getLedBrightness().serialize(object);

  String json;
  serializeJson(document, json);
  request->send(200, "application/json", json);
}

void EarsEndpoint::handlePut(AsyncWebServerRequest *request)
{
  bool updated = false;
  if (request->hasParam("brightnessPercent", true)) {
    const float percent = request->getParam("brightnessPercent", true)->value().toFloat();
    if (percent > 100.0f || percent < 0.0f) {
      request->send(400, "text/plain", F("Could not set brightness use 0-100 percent."));
      return;
    }
    brightnessController_.setBrightnessPercent(percent);
    updated = true;
  } else if (request->hasParam("brightness", true)) {
    const int brightness = request->getParam("brightness", true)->value().toInt();
    if (brightness >= 256 || brightness < 0) {
      request->send(400, "text/plain", F("Could not set brightness use 0-255 value."));
      return;
    }
    brightnessController_.setBrightness(static_cast<uint8_t>(brightness));
    updated = true;
  }

  if (!updated) {
    request->send(400, "text/plain", F("No valid parameters detected!"));
    return;
  }
  request->send(200, "text/plain", F("Brightness set."));
  if (onSettingsChanged_) onSettingsChanged_();
}
