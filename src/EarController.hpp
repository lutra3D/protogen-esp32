#ifndef EAR_CONTROLLER_HPP
#define EAR_CONTROLLER_HPP

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <ArduinoJson.h>

#include "Model/Ear.hpp"

class EarController {
public:
  EarController(uint16_t ledCount, uint8_t dataPin);

  bool begin();
  void setBrightness(uint8_t brightness);
  void setBrightnessPercent(float percent);
  uint8_t getBrightness() const;

  void setColor(Color color);
  void setGradient(Gradient gradient);

  float getBrightnessPercent() const;

  Ear &getEar();

  void update();

private:
  uint16_t ledCount_;
  Adafruit_NeoPixel earLeds_;
  Ear ear_;
  CircleDisplay display_;
};

#endif // EAR_CONTROLLER_HPP
