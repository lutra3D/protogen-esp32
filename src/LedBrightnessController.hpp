#ifndef LED_BRIGHTNESS_CONTROLLER_HPP
#define LED_BRIGHTNESS_CONTROLLER_HPP

#include "Model/LedBrightness.hpp"

class LedBrightnessController {
public:
  LedBrightnessController();

  void setBrightness(uint8_t brightness);
  void setBrightnessPercent(float percent);
  uint8_t getBrightness() const;
  float getBrightnessPercent() const;

  LedBrightness &getLedBrightness();
  const LedBrightness &getLedBrightness() const;

private:
  LedBrightness ledBrightness_;
};

#endif
