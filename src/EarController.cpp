#include "EarController.hpp"

#include <math.h>

namespace {
constexpr float kMaxBrightnessPercent = 100.0f;
constexpr float kMinBrightnessPercent = 0.0f;
constexpr float kPercentToByte = 255.0f / 100.0f;
}

void Ear::Color::set(uint8_t r, uint8_t g, uint8_t b) {
  red = r;
  green = g;
  blue = b;
}

String Ear::Color::toHexString() const {
  char buffer[8];
  snprintf(buffer, sizeof(buffer), "#%02x%02x%02x", red, green, blue);
  return String(buffer);
}

bool Ear::Color::setFromHex(const String &hex) {
  if (hex.length() != 7 || hex.charAt(0) != '#') {
    return false;
  }

  auto parseComponent = [&hex](uint8_t start) -> int {
    String component = hex.substring(start, start + 2);
    long value = strtol(component.c_str(), nullptr, 16);
    if (value < 0 || value > 255) {
      return -1;
    }
    return static_cast<int>(value);
  };

  const int r = parseComponent(1);
  const int g = parseComponent(3);
  const int b = parseComponent(5);
  if (r < 0 || g < 0 || b < 0) {
    return false;
  }

  set(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b));
  return true;
}

Ear::Brightness::Brightness(uint8_t value) : value_(value) {}

void Ear::Brightness::setValue(uint8_t value) {
  value_ = value;
}

uint8_t Ear::Brightness::getValue() const {
  return value_;
}

void Ear::Brightness::setPercent(float percent) {
  float clamped = percent;
  if (clamped < kMinBrightnessPercent) {
    clamped = kMinBrightnessPercent;
  } else if (clamped > kMaxBrightnessPercent) {
    clamped = kMaxBrightnessPercent;
  }
  value_ = static_cast<uint8_t>(roundf(clamped * kPercentToByte));
}

float Ear::Brightness::getPercent() const {
  return (static_cast<float>(value_) / 255.0f) * 100.0f;
}

Ear::Ear() : color_(), brightness_() {}

void Ear::setColor(uint8_t red, uint8_t green, uint8_t blue) {
  color_.set(red, green, blue);
}

bool Ear::setColorFromHex(const String &hex) {
  return color_.setFromHex(hex);
}

const Ear::Color &Ear::getColor() const {
  return color_;
}

String Ear::getColorHexString() const {
  return color_.toHexString();
}

void Ear::setBrightness(uint8_t brightness) {
  brightness_.setValue(brightness);
}

void Ear::setBrightnessPercent(float percent) {
  brightness_.setPercent(percent);
}

bool Ear::setBrightnessPercentChecked(float percent) {
  if (percent < kMinBrightnessPercent || percent > kMaxBrightnessPercent) {
    return false;
  }
  setBrightnessPercent(percent);
  return true;
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

  json["color"] = getColorHexString();
  json["brightness"] = getBrightness();
  json["brightnessPercent"] = getBrightnessPercent();
}

EarController::EarController(uint16_t ledCount, uint8_t dataPin)
    : ledCount_(ledCount),
      earLeds_(ledCount, dataPin, NEO_GRB + NEO_KHZ800),
      ear_() {}

bool EarController::begin() {
  earLeds_.begin();
  earLeds_.setBrightness(ear_.getBrightness());
  return true;
}

void EarController::setBrightness(uint8_t brightness) {
  ear_.setBrightness(brightness);
}

void EarController::setBrightnessPercent(float percent) {
  ear_.setBrightnessPercent(percent);
}

uint8_t EarController::getBrightness() const {
  return ear_.getBrightness();
}

void EarController::setColor(uint8_t red, uint8_t green, uint8_t blue) {
  ear_.setColor(red, green, blue);
}

String EarController::getColorHexString() const {
  return ear_.getColorHexString();
}

float EarController::getBrightnessPercent() const {
  return ear_.getBrightnessPercent();
}

Ear &EarController::getEar() {
  return ear_;
}

void EarController::update() {
  const auto &color = ear_.getColor();
  earLeds_.setBrightness(ear_.getBrightness());
  for (uint16_t index = 0; index < ledCount_; ++index) {
    earLeds_.setPixelColor(index, Adafruit_NeoPixel::Color(color.red, color.green, color.blue));
  }
  earLeds_.show();
}
