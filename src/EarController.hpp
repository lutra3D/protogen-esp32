#ifndef EAR_CONTROLLER_HPP
#define EAR_CONTROLLER_HPP

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#include "LedBrightnessController.hpp"
#include "Model/Ear.hpp"

class EarController {
public:
  EarController(uint16_t ledCount, uint8_t dataPin, LedBrightnessController &brightnessController);

  bool begin();
  void setColor(Color color);
  void setGradient(Gradient gradient);
  Ear &getEar();
  void update();

private:
  uint16_t ledCount_;
  Adafruit_NeoPixel earLeds_;
  Ear ear_;
  CircleDisplay display_;
  LedBrightnessController &brightnessController_;
};

#endif // EAR_CONTROLLER_HPP
