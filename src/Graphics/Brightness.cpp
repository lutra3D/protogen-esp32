#include "Brightness.hpp"

Brightness::Brightness(uint8_t value) : value_(value) {}

void Brightness::setValue(uint8_t value) {
  value_ = value;
}

uint8_t Brightness::getValue() const {
  return value_;
}

void Brightness::setPercent(float percent) {
  float clamped = percent;
  if (clamped < kMinBrightnessPercent) {
    clamped = kMinBrightnessPercent;
  } else if (clamped > kMaxBrightnessPercent) {
    clamped = kMaxBrightnessPercent;
  }
  value_ = static_cast<uint8_t>(roundf(clamped * kPercentToByte));
}

float Brightness::getPercent() const {
  return (static_cast<float>(value_) / 255.0f) * 100.0f;
}
