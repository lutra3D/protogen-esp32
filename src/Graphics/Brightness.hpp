#ifndef BRIGHTNESS_HPP
#define BRIGHTNESS_HPP

#include <Arduino.h>

class Brightness {
  public:
    const float kMaxBrightnessPercent = 100.0f;
    const float kMinBrightnessPercent = 0.0f;
    const float kPercentToByte = 255.0f / 100.0f;

    explicit Brightness(uint8_t value = 80);

    void setValue(uint8_t value);
    uint8_t getValue() const;
    void setPercent(float percent);
    float getPercent() const;
    
  private:
    uint8_t value_;
  };

#endif // BRIGHTNESS_HPP
