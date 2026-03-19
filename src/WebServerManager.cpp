#include "WebServerManager.hpp"

#include <ArduinoJson.h>
#include <FS.h>
#include <vector>

namespace
{
  bool parseEmotionDefinitionJson(const String &jsonPayload, EmotionDefinition &emotion, String &error)
  {
    JsonDocument doc;
    DeserializationError parseError = deserializeJson(doc, jsonPayload);
    if (parseError)
    {
      error = "Invalid JSON payload.";
      return false;
    }

    JsonVariant root = doc.as<JsonVariant>();
    if (!root.is<JsonObject>())
    {
      error = "Payload must be an object.";
      return false;
    }

    return emotion.deserialize(root.as<JsonObject>(), error);
  }

  void applyEmotionEarColor(EarController &earController, const EmotionDefinition *emotion)
  {
    if (emotion == nullptr)
    {
      return;
    }

    Ear &ear = earController.getEar();
    if (emotion->earColorMode == ColorMode::Gradient)
    {
      Serial.println("[D] Setting ear gradient");
      ear.setGradient(emotion->earGradient);
    }
    else
    {
      Serial.println("[D] Setting ear color");
      ear.setColor(emotion->earColor);
    }
  }
} // namespace

WebServerManager::WebServerManager(
    EmotionState &emotionState,
    FanController &fanController,
    EarController &earController,
    TiltController &tiltController,
    AnimationManager &animationManager)
    : server_(80),
      emotionState_(emotionState),
      fanController_(fanController),
      earController_(earController),
      tiltController_(tiltController),
      animationManager_(animationManager) {}

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
  server_.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server_.on(
      "/file", HTTP_POST,
      [](AsyncWebServerRequest *request)
      {
        auto *context = static_cast<UploadContext *>(request->_tempObject);
        if (context == nullptr)
        {
          request->send(400, "text/plain", F("No file uploaded."));
          return;
        }

        String message = context->message;
        if (!message.length())
        {
          message = F("File was saved.");
        }

        if (context->error)
        {
          request->send(400, "text/plain", message);
        }
        else
        {
          request->send(200, "text/plain", message);
        }

        delete context;
        request->_tempObject = nullptr;
      },
      [this](AsyncWebServerRequest *request, String filename, size_t index,
             uint8_t *data, size_t len, bool final)
      {
        Serial.println(F("[I] File upload callback called"));
        handleFileUpload(request, filename, index, data, len, final);
      });

  server_.on("/file", HTTP_DELETE, [this](AsyncWebServerRequest *request)
             {
    if (!request->hasParam("file")) {
      request->send(400, "text/plain", F("Parameter 'file' not present!"));
      return;
    }

    auto filePath = request->getParam("file")->value();
    Serial.println("[I] Deleting " + String(filePath));

    if(!SPIFFS.exists(filePath)){
      request->send(400, "text/plain", F("File does not exist!"));
      return;
    }

    if(!filePath.startsWith("/anims/")){
      request->send(400, "text/plain", F("Cannot delete the file, file is not an animation!"));
      return;
    }

    SPIFFS.remove(filePath);
    request->send(200, "text/plain", F("File was deleted.")); });

  server_.on("/files", HTTP_GET, [this](AsyncWebServerRequest *request)
             {
              auto emotionFiles = animationManager_.getEmotions();

    JsonDocument doc;
    JsonArray array = doc.to<JsonArray>();

    for (const auto &file : emotionFiles) {
      JsonObject fileObject = array.add<JsonObject>();
      file.serialize(fileObject);
    }
    
    String json;
    serializeJson(doc, json);

    request->send(200, "application/json", json); });

  server_.on("/emotions", HTTP_GET, [this](AsyncWebServerRequest *request)
             {
    JsonDocument doc;
    JsonArray array = doc.to<JsonArray>();
    for (const auto &emotion : emotionState_.getEmotionDefinitions()) {
      JsonObject emotionObject = array.add<JsonObject>();
      emotion.serialize(emotionObject);
    }

    String json;
    serializeJson(doc, json);
    request->send(200, "application/json", json); });

  server_.on("/emotion", HTTP_GET, [this](AsyncWebServerRequest *request)
             {
    JsonDocument doc;
    JsonObject object = doc.to<JsonObject>();
    const auto *emotion = emotionState_.getCurrentEmotionDefinition();
    emotion->serialize(object);

    String json;
    serializeJson(doc, json);
    request->send(200, "application/json", json); });

  server_.on("/emotion", HTTP_POST, [this](AsyncWebServerRequest *request)
             {
    if (!request->hasParam("plain", true)) {
      request->send(400, "text/plain", F("JSON payload is required."));
      return;
    }

    EmotionDefinition emotion;

    String error;
    if (!parseEmotionDefinitionJson(request->getParam("plain", true)->value(), emotion, error)) {
      request->send(400, "text/plain", error);
      return;
    }
    
    if (!emotionState_.upsertEmotionDefinition(emotion, false)) {
      request->send(409, "text/plain", F("Emotion already exists."));
      return;
    }

    request->send(201, "text/plain", F("Emotion created.")); });

  server_.on("/emotion", HTTP_PUT, [this](AsyncWebServerRequest *request)
             {
    if (request->hasParam("name", true)) {
      emotionState_.setCurrentEmotion(request->getParam("name", true)->value());

      applyEmotionEarColor(earController_, emotionState_.getCurrentEmotionDefinition());
      request->send(200, "text/plain", F("Emotion changed."));
      return;
    }

    if (!request->hasParam("plain", true)) {
      request->send(400, "text/plain", F("JSON payload is required for emotion update."));
      return;
    }

    EmotionDefinition emotion;
    String error;
    if (!parseEmotionDefinitionJson(request->getParam("plain", true)->value(), emotion, error)) {
      request->send(400, "text/plain", error);
      return;
    }

    if (emotionState_.getEmotionDefinitionByName(emotion.name) == nullptr &&
        emotionState_.getEmotionDefinitionByPath(emotion.path) == nullptr) {
      request->send(404, "text/plain", F("Emotion does not exist."));
      return;
    }

    emotionState_.upsertEmotionDefinition(emotion, true);

    if (emotionState_.getCurrentEmotion() == emotion.path ||
        (emotionState_.getCurrentEmotionDefinition() != nullptr &&
         emotionState_.getCurrentEmotionDefinition()->name == emotion.name)) {
      applyEmotionEarColor(earController_, emotionState_.getCurrentEmotionDefinition());
    }

    request->send(200, "text/plain", F("Emotion updated.")); });

  server_.on("/emotion", HTTP_DELETE, [this](AsyncWebServerRequest *request)
             {
    if (!request->hasParam("name")) {
      request->send(400, "text/plain", F("Parameter 'name' is required."));
      return;
    }

    String name = request->getParam("name")->value();
    if (!emotionState_.removeEmotionDefinitionByName(name)) {
      request->send(404, "text/plain", F("Emotion not found."));
      return;
    }

    request->send(200, "text/plain", F("Emotion removed.")); });

  server_.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request)
             { request->send(200, "text/plain", String(ESP.getFreeHeap())); });

  server_.on("/fan", HTTP_GET, [this](AsyncWebServerRequest *request)
             { request->send(200, "text/plain", String(fanController_.getDutyCycle())); });

  server_.on("/fan", HTTP_PUT, [this](AsyncWebServerRequest *request)
             {
    if (request->hasParam("duty", true)) {
      int duty = request->getParam("duty", true)->value().toInt();
      if (fanController_.setDutyCycle(duty)) {
        request->send(200, "text/plain", "Set PWM to: " + String(fanController_.getDutyCycle()));
        return;
      }
    }
    request->send(400, "text/plain", F("Invalid duty cycle")); });

  server_.on("/ears", HTTP_GET, [this](AsyncWebServerRequest *request)
             {
    JsonDocument doc;
    JsonObject obj = doc.to<JsonObject>();
    earController_.getEar().serialize(obj);

    String json;
    serializeJson(doc, json);
    request->send(200, "application/json", json); });

  server_.on("/ears", HTTP_PUT, [this](AsyncWebServerRequest *request)
             {
    bool updated = false;

    Ear &ear = earController_.getEar();

    if (request->hasParam("color", true)) {
      String color = request->getParam("color", true)->value();
      if (!ear.setColorFromHex(color)) {
        request->send(400, "text/plain", F("Could not set color use #FFFFFF format."));
        return;
      }
      updated = true;
    }

    if (request->hasParam("brightnessPercent", true)) {
      const float percent = request->getParam("brightnessPercent", true)->value().toFloat();
      if (percent >= 100 || percent < 0) {
         request->send(400, "text/plain", F("Could not set brightness use 0-100 percent."));
         return;
      }
      ear.setBrightnessPercent(percent);
      updated = true;
    } else if (request->hasParam("brightness", true)) {
      int brightness = request->getParam("brightness", true)->value().toInt();
      if (brightness >= 256 || brightness < 0) {
        request->send(400, "text/plain", F("Could not set brightness use 0-255 value."));
        return;
      }
      ear.setBrightness(static_cast<uint8_t>(brightness));
      updated = true;
    }

    if (!updated) {
      request->send(400, "text/plain", F("No valid parameters detected!"));
      return;
    }

    request->send(200, "text/plain", F("Color/brightness set.")); });

  server_.on("/gyro", HTTP_GET, [this](AsyncWebServerRequest *request)
             {
    if (!tiltController_.isEnabled()) {
      request->send(200, "text/plain", F("Tilt is disabled"));
      return;
    }
    request->send(200, "text/plain", tiltController_.readAcceleration()); });

  server_.onNotFound([](AsyncWebServerRequest *request)
                     { request->send(404, "text/plain", "Not found"); });
}

void WebServerManager::handleFileUpload(AsyncWebServerRequest *request,
                                        String filename, size_t index,
                                        uint8_t *data, size_t len,
                                        bool final)
{
  auto *context = static_cast<UploadContext *>(request->_tempObject);

  if (index == 0)
  {
    if (context != nullptr)
    {
      delete context;
      context = nullptr;
    }

    context = new UploadContext();
    request->_tempObject = context;

    filename.trim();
    if (filename.isEmpty())
    {
      context->error = true;
      context->message = F("File name is required.");
      return;
    }

    for (size_t i = 0; i < filename.length(); ++i)
    {
      char c = filename.charAt(i);
      if (c == '/' || c == '\\')
      {
        filename.setCharAt(i, '_');
      }
    }

    context->targetPath = "/anims/" + filename;
    context->tempPath = context->targetPath + F(".tmp");

    if (SPIFFS.exists(context->tempPath))
    {
      SPIFFS.remove(context->tempPath);
    }

    request->_tempFile = SPIFFS.open(context->tempPath, FILE_WRITE);
    if (!request->_tempFile)
    {
      context->error = true;
      context->message = F("Error opening temporary file for writing!");
      return;
    }
  }

  if (context == nullptr)
  {
    return;
  }

  if (context->error)
  {
    if (final && request->_tempFile)
    {
      request->_tempFile.close();
      request->_tempFile = File();
    }
    return;
  }

  if (len > 0 && request->_tempFile)
  {
    size_t written = request->_tempFile.write(data, len);
    if (written != len && !context->error)
    {
      context->error = true;
      context->message = F("Failed to write uploaded data.");
      request->_tempFile.close();
      request->_tempFile = File();
      if (SPIFFS.exists(context->tempPath))
      {
        SPIFFS.remove(context->tempPath);
      }
    }
  }

  if (!final)
  {
    return;
  }

  if (request->_tempFile)
  {
    request->_tempFile.close();
    request->_tempFile = File();
  }

  if (context->error)
  {
    if (SPIFFS.exists(context->tempPath))
    {
      SPIFFS.remove(context->tempPath);
    }
    return;
  }

  if (SPIFFS.exists(context->targetPath))
  {
    SPIFFS.remove(context->targetPath);
  }

  if (!SPIFFS.rename(context->tempPath, context->targetPath))
  {
    context->error = true;
    context->message = F("Failed to save file.");
    if (SPIFFS.exists(context->tempPath))
    {
      SPIFFS.remove(context->tempPath);
    }
    return;
  }

  context->message = F("File was saved.");
}
