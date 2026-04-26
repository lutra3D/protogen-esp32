#include "NeopixelFaceDisplay.hpp"

namespace {
uint8_t Expand5to8(uint16_t value)
{
  return static_cast<uint8_t>((value * 527U + 23U) >> 6);
}

uint8_t Expand6to8(uint16_t value)
{
  return static_cast<uint8_t>((value * 259U + 33U) >> 6);
}
} // namespace

NeopixelFaceDisplay::NeopixelFaceDisplay(uint16_t width, uint16_t height, uint8_t leftPin,
                                         uint8_t rightPin)
    : AbstractFaceDisplay(width * 2, height),
      panelWidth_(width),
      panelHeight_(height),
      pixelsPerPanel_(width * height),
      leftPin_(leftPin),
      rightPin_(rightPin),
      leftDisplay_(pixelsPerPanel_, leftPin_, NEO_GRB + NEO_KHZ800),
      rightDisplay_(pixelsPerPanel_, rightPin_, NEO_GRB + NEO_KHZ800)
{
}

NeopixelFaceDisplay::~NeopixelFaceDisplay() {}

bool NeopixelFaceDisplay::initializeDisplay()
{
  leftDisplay_.begin();
  rightDisplay_.begin();
  clearFrame();
  presentFrame();
  return true;
}

uint16_t NeopixelFaceDisplay::mapCoordinatesToIndex(uint16_t x, uint16_t y) const
{
  if (x >= panelWidth_ || y >= panelHeight_)
  {
    return 0;
  }

  if ((y & 1U) == 0)
  {
    return y * panelWidth_ + x;
  }

  return y * panelWidth_ + (panelWidth_ - 1 - x);
}

void NeopixelFaceDisplay::drawPixel(int x, int y, uint16_t color565)
{
  if (x < 0 || y < 0 || y >= panelHeight_ || x >= width_)
  {
    return;
  }

  const bool isRightPanel = (x >= panelWidth_);
  const uint16_t panelX = static_cast<uint16_t>(isRightPanel ? x - panelWidth_ : x);
  const uint16_t index = mapCoordinatesToIndex(panelX, static_cast<uint16_t>(y));

  const uint8_t red = Expand5to8((color565 >> 11) & 0x1F);
  const uint8_t green = Expand6to8((color565 >> 5) & 0x3F);
  const uint8_t blue = Expand5to8(color565 & 0x1F);
  const uint32_t color = Adafruit_NeoPixel::Color(red, green, blue);

  if (isRightPanel)
  {
    rightDisplay_.setPixelColor(index, color);
  }
  else
  {
    leftDisplay_.setPixelColor(index, color);
  }
}

void NeopixelFaceDisplay::clearFrame()
{
  leftDisplay_.clear();
  rightDisplay_.clear();
}

void NeopixelFaceDisplay::presentFrame()
{
  leftDisplay_.show();
  rightDisplay_.show();
}

void NeopixelFaceDisplay::showBootScreen()
{
  clearFrame();
  presentFrame();
}
