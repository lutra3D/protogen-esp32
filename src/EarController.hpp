#ifndef EAR_CONTROLLER_HPP
#define EAR_CONTROLLER_HPP

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

class EarController {
public:
  EarController(uint16_t ledCount, uint8_t dataPin);

  bool begin();
  void setBrightness(uint8_t brightness);
  uint8_t getBrightness() const;

  void setColor(uint8_t red, uint8_t green, uint8_t blue);
  uint8_t getRed() const;
  uint8_t getGreen() const;
  uint8_t getBlue() const;

  String getColorHexString() const;
  float getBrightnessPercent() const;

  void update();

private:
  uint16_t ledCount_;
  Adafruit_NeoPixel earLeds_;
  uint8_t brightness_;
  uint8_t red_;
  uint8_t green_;
  uint8_t blue_;
};

#endif // EAR_CONTROLLER_HPP
