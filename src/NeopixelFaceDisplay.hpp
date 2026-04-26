#ifndef NEOPIXEL_FACE_DISPLAY_HPP
#define NEOPIXEL_FACE_DISPLAY_HPP

#include <Adafruit_NeoPixel.h>

#include "AbstractFaceDisplay.hpp"

class NeopixelFaceDisplay : public AbstractFaceDisplay {
public:
  NeopixelFaceDisplay(uint16_t width, uint16_t height, uint8_t leftPin, uint8_t rightPin);
  ~NeopixelFaceDisplay() override;

  uint16_t mapCoordinatesToIndex(uint16_t x, uint16_t y) const;

protected:
  bool initializeDisplay() override;
  void drawPixel(int x, int y, uint16_t color565) override;
  void clearFrame() override;
  void presentFrame() override;
  void showBootScreen() override;

private:
  uint16_t panelWidth_;
  uint16_t panelHeight_;
  uint16_t pixelsPerPanel_;
  uint8_t leftPin_;
  uint8_t rightPin_;

  Adafruit_NeoPixel leftDisplay_;
  Adafruit_NeoPixel rightDisplay_;
};

#endif // NEOPIXEL_FACE_DISPLAY_HPP
