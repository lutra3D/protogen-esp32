#ifndef LED_BRIGHTNESS_HPP
#define LED_BRIGHTNESS_HPP

#include <ArduinoJson.h>

#include "../Graphics/Brightness.hpp"

class LedBrightness {
public:
  LedBrightness();

  void setBrightness(uint8_t brightness);
  void setBrightnessPercent(float percent);
  uint8_t getBrightness() const;
  float getBrightnessPercent() const;

  void serialize(JsonVariant json) const;
  bool deserialize(const JsonObject &object, String &error);

private:
  Brightness brightness_;
};

#endif
