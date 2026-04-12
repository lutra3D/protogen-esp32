#include "WebEndpoints/Ears/EarsEndpoint.hpp"

#include <ArduinoJson.h>

EarsEndpoint::EarsEndpoint(EarController &earController,
                           std::function<void()> onSettingsChanged)
    : earController_(earController),
      onSettingsChanged_(onSettingsChanged)
{
}

void EarsEndpoint::registerEndpoint(AsyncWebServer &server)
{
  server.on("/ears", HTTP_GET, [this](AsyncWebServerRequest *request)
            { handleGet(request); });
  server.on("/ears", HTTP_PUT, [this](AsyncWebServerRequest *request)
            { handlePut(request); });
}

void EarsEndpoint::handleGet(AsyncWebServerRequest *request)
{
  JsonDocument document;
  JsonObject object = document.to<JsonObject>();
  earController_.getEar().serialize(object);

  String json;
  serializeJson(document, json);
  request->send(200, "application/json", json);
}

void EarsEndpoint::handlePut(AsyncWebServerRequest *request)
{
  bool updated = false;
  Ear &ear = earController_.getEar();

  if (!updateBrightness(request, ear, updated))
  {
    return;
  }

  if (!updated)
  {
    request->send(400, "text/plain", F("No valid parameters detected!"));
    return;
  }

  request->send(200, "text/plain", F("Brightness set."));
  if (onSettingsChanged_)
  {
    onSettingsChanged_();
  }
}

bool EarsEndpoint::updateBrightness(AsyncWebServerRequest *request, Ear &ear,
                                    bool &updated)
{
  if (request->hasParam("brightnessPercent", true))
  {
    const float percent =
        request->getParam("brightnessPercent", true)->value().toFloat();
    if (percent >= 100 || percent < 0)
    {
      request->send(400, "text/plain",
                    F("Could not set brightness use 0-100 percent."));
      return false;
    }

    ear.setBrightnessPercent(percent);
    updated = true;
    return true;
  }

  if (!request->hasParam("brightness", true))
  {
    return true;
  }

  const int brightness = request->getParam("brightness", true)->value().toInt();
  if (brightness >= 256 || brightness < 0)
  {
    request->send(400, "text/plain",
                  F("Could not set brightness use 0-255 value."));
    return false;
  }

  ear.setBrightness(static_cast<uint8_t>(brightness));
  updated = true;
  return true;
}
