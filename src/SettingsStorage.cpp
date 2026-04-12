#include "SettingsStorage.hpp"

#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>

SettingsStorage::SettingsStorage(EmotionState &emotionState,
                                 FanController &fanController,
                                 EarController &earController)
    : emotionState_(emotionState),
      fanController_(fanController),
      earController_(earController)
{
}

bool SettingsStorage::load()
{
  if (!SPIFFS.exists(kSettingsPath))
  {
    Serial.println(F("[I] No settings file found. Using defaults."));
    return true;
  }

  File file = SPIFFS.open(kSettingsPath, FILE_READ);
  if (!file)
  {
    Serial.println(F("[E] Could not open settings file for reading."));
    return false;
  }

  JsonDocument document;
  const DeserializationError error = deserializeJson(document, file);
  file.close();

  if (error)
  {
    Serial.printf("[E] Failed to parse settings JSON: %s\n", error.c_str());
    return false;
  }

  if (document["emotions"].is<JsonArray>())
  {
    std::vector<EmotionDefinition> loadedEmotions;
    for (JsonObject emotionObject : document["emotions"].as<JsonArray>())
    {
      EmotionDefinition emotion;
      String emotionError;
      if (!emotion.deserialize(emotionObject, emotionError))
      {
        Serial.printf("[E] Invalid emotion in settings: %s\n", emotionError.c_str());
        return false;
      }
      loadedEmotions.push_back(emotion);
    }
    emotionState_.seedEmotionDefinitions(loadedEmotions);
  }

  if (document["currentEmotion"].is<String>())
  {
    emotionState_.setCurrentEmotion(document["currentEmotion"].as<String>());
  }

  if (document["fan"].is<JsonObject>())
  {
    JsonObject fan = document["fan"].as<JsonObject>();
    if (fan["dutyCycle"].is<int>())
    {
      const int dutyCycle = fan["dutyCycle"].as<int>();
      if (!fanController_.setDutyCycle(dutyCycle))
      {
        Serial.printf("[E] Invalid fan dutyCycle in settings: %d\n", dutyCycle);
        return false;
      }
    }
  }

  if (document["ear"].is<JsonObject>())
  {
    String earError;
    if (!earController_.getEar().deserialize(document["ear"].as<JsonObject>(), earError))
    {
      Serial.printf("[E] Invalid ear settings: %s\n", earError.c_str());
      return false;
    }
  }

  Serial.println(F("[I] Settings loaded."));
  return true;
}

bool SettingsStorage::save() const
{
  JsonDocument document;

  JsonObject earObject = document["ear"].to<JsonObject>();
  earController_.getEar().serialize(earObject);

  JsonArray emotionsArray = document["emotions"].to<JsonArray>();
  for (const auto &emotion : emotionState_.getEmotionDefinitions())
  {
    JsonObject emotionObject = emotionsArray.add<JsonObject>();
    emotion.serialize(emotionObject);
  }

  document["currentEmotion"] = emotionState_.getCurrentEmotion();

  JsonObject fanObject = document["fan"].to<JsonObject>();
  fanObject["dutyCycle"] = fanController_.getDutyCycle();

  File file = SPIFFS.open(kSettingsPath, FILE_WRITE);
  if (!file)
  {
    Serial.println(F("[E] Could not open settings file for writing."));
    return false;
  }

  if (serializeJson(document, file) == 0)
  {
    Serial.println(F("[E] Failed to write settings JSON."));
    file.close();
    return false;
  }

  file.close();
  Serial.println(F("[I] Settings saved."));
  return true;
}
