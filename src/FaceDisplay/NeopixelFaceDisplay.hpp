#ifndef NEOPIXELFACEDISPLAY_HPP
#define NEOPIXELFACEDISPLAY_HPP

#include <Adafruit_NeoPixel.h>

#include "GifFaceDisplay.hpp"

class NeopixelFaceDisplay : public GifFaceDisplay
{
public:
  NeopixelFaceDisplay(uint8_t leftPin, uint8_t rightPin, uint16_t panelWidth, uint16_t panelHeight);
  ~NeopixelFaceDisplay() override;

  bool begin() override;
  bool displayReady() const override;
  void drawPixel(int x, int y, Color color) override;

protected:
  void afterFrameRendered() override;

private:
  uint16_t getPixelIndex(uint16_t x, uint16_t y) const;

  uint8_t leftPin_;
  uint8_t rightPin_;
  uint16_t panelWidth_;
  uint16_t panelHeight_;
  uint16_t pixelCountPerPanel_;

  Adafruit_NeoPixel leftPanel_;
  Adafruit_NeoPixel rightPanel_;

  bool initialized_;
};

#endif // NEOPIXELFACEDISPLAY_HPP
