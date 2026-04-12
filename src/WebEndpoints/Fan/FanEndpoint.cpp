#include "WebEndpoints/Fan/FanEndpoint.hpp"

FanEndpoint::FanEndpoint(FanController &fanController,
                         std::function<void()> onSettingsChanged)
    : fanController_(fanController),
      onSettingsChanged_(onSettingsChanged)
{
}

void FanEndpoint::registerEndpoint(AsyncWebServer &server)
{
  server.on("/fan", HTTP_GET, [this](AsyncWebServerRequest *request)
            { handleGet(request); });
  server.on("/fan", HTTP_PUT, [this](AsyncWebServerRequest *request)
            { handlePut(request); });
}

void FanEndpoint::handleGet(AsyncWebServerRequest *request)
{
  request->send(200, "text/plain", String(fanController_.getDutyCycle()));
}

void FanEndpoint::handlePut(AsyncWebServerRequest *request)
{
  if (request->hasParam("duty", true))
  {
    const int duty = request->getParam("duty", true)->value().toInt();
    if (fanController_.setDutyCycle(duty))
    {
      request->send(200, "text/plain",
                    "Set PWM to: " + String(fanController_.getDutyCycle()));
      if (onSettingsChanged_)
      {
        onSettingsChanged_();
      }
      return;
    }
  }

  request->send(400, "text/plain", F("Invalid duty cycle"));
}
