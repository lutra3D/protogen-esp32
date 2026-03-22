#include "Ear.hpp"

Ear::Ear() : color_(0,0,0), gradient_(), colorMode_(ColorMode::Solid), brightness_() {}

void Ear::setColor(uint8_t red, uint8_t green, uint8_t blue) {
  color_.set(red, green, blue);
  colorMode_ = ColorMode::Solid;
}

void Ear::setColor(const Color &color)
{
  color_ = color;
  colorMode_ = ColorMode::Solid;
}

bool Ear::setColorFromHex(const String &hex) {
  if (!color_.setFromHex(hex)) {
    return false;
  }
  colorMode_ = ColorMode::Solid;
  return true;
}

const Color &Ear::getColor() const {
  return color_;
}

String Ear::getColorHexString() const {
  return color_.toHexString();
}

bool Ear::setGradientFromHex(const String &fromHex, const String &toHex, float angle, float midpoint)
{
    if (!gradient_.setFromHex(fromHex, toHex, angle, midpoint)) {
        return false;
    }
    colorMode_ = ColorMode::Solid;
    return true;
}

void Ear::setGradient(const Gradient &gradient)
{
  gradient_ = gradient;
  colorMode_ = ColorMode::Gradient;
}

const Gradient &Ear::getGradient() const
{
  return gradient_;
}

ColorMode Ear::getColorMode() const
{
  return colorMode_;
}

void Ear::setBrightness(uint8_t brightness) {
  brightness_.setValue(brightness);
}

void Ear::setBrightnessPercent(float percent) {
  brightness_.setPercent(percent);
}

uint8_t Ear::getBrightness() const {
  return brightness_.getValue();
}

float Ear::getBrightnessPercent() const {
  return brightness_.getPercent();
}

void Ear::serialize(JsonVariant json) const {
  if (json.isNull()) {
    return;
  }

  json["mode"] = colorMode_ == ColorMode::Gradient ? "gradient" : "solid";
  json["color"] = getColorHexString();
  json["brightness"] = getBrightness();
  json["brightnessPercent"] = getBrightnessPercent();

  JsonObject gradientJson = json["gradient"].to<JsonObject>();
  gradient_.serialize(gradientJson);
}