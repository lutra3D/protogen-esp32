#ifndef EAR_HPP
#define EAR_HPP

#include <ArduinoJson.h>

#include "../Graphics/Brightness.hpp"
#include "../Graphics/Color.hpp"
#include "../Graphics/Gradient.hpp"

class Ear {
public:
  Ear();

  void setColor(uint8_t red, uint8_t green, uint8_t blue);
  void setColor(const Color &color);
  bool setColorFromHex(const String &hex);
  const Color &getColor() const;
  String getColorHexString() const;
  bool setGradientFromHex(const String &fromHex, const String &toHex, float angle, float midpoint);
  void setGradient(const Gradient &gradient);
  const Gradient &getGradient() const;

  void setColorMode(ColorMode mode);
  ColorMode getColorMode() const;

  void setBrightness(uint8_t brightness);
  void setBrightnessPercent(float percent);
  uint8_t getBrightness() const;
  float getBrightnessPercent() const;

  void serialize(JsonVariant json) const;

private:
  Color color_;
  Gradient gradient_;
  ColorMode colorMode_;
  Brightness brightness_;
};

#endif