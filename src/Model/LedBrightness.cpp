#include "LedBrightness.hpp"

LedBrightness::LedBrightness() : brightness_() {}

void LedBrightness::setBrightness(uint8_t brightness) { brightness_.setValue(brightness); }
void LedBrightness::setBrightnessPercent(float percent) { brightness_.setPercent(percent); }
uint8_t LedBrightness::getBrightness() const { return brightness_.getValue(); }
float LedBrightness::getBrightnessPercent() const { return brightness_.getPercent(); }

void LedBrightness::serialize(JsonVariant json) const {
  if (json.isNull()) return;
  json["brightness"] = getBrightness();
  json["brightnessPercent"] = getBrightnessPercent();
}

bool LedBrightness::deserialize(const JsonObject &object, String &error) {
  if (object["brightness"].is<int>()) {
    const int brightness = object["brightness"].as<int>();
    if (brightness < 0 || brightness > 255) {
      error = F("Brightness must be between 0 and 255.");
      return false;
    }
    setBrightness(static_cast<uint8_t>(brightness));
    return true;
  }

  if (object["brightnessPercent"].is<float>()) {
    const float brightnessPercent = object["brightnessPercent"].as<float>();
    if (brightnessPercent < 0.0f || brightnessPercent > 100.0f) {
      error = F("Brightness percent must be between 0 and 100.");
      return false;
    }
    setBrightnessPercent(brightnessPercent);
    return true;
  }

  return true;
}
