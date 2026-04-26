#include "NeopixelFaceDisplay.hpp"

NeopixelFaceDisplay::NeopixelFaceDisplay(uint8_t leftPin, uint8_t rightPin, uint16_t panelWidth, uint16_t panelHeight)
    : GifFaceDisplay(),
      leftPin_(leftPin),
      rightPin_(rightPin),
      panelWidth_(panelWidth),
      panelHeight_(panelHeight),
      pixelCountPerPanel_(panelWidth * panelHeight),
      leftPanel_(pixelCountPerPanel_, leftPin, NEO_GRB + NEO_KHZ800),
      rightPanel_(pixelCountPerPanel_, rightPin, NEO_GRB + NEO_KHZ800),
      initialized_(false)
{
}

NeopixelFaceDisplay::~NeopixelFaceDisplay()
{
  closeEmotion();
}

bool NeopixelFaceDisplay::begin()
{
  Serial.println("[I] Initializing NeopixelFaceDisplay...");

  if (panelWidth_ == 0 || panelHeight_ == 0)
  {
    Serial.println("[E] Invalid neopixel panel dimensions.");
    return false;
  }

  leftPanel_.begin();
  rightPanel_.begin();

  leftPanel_.clear();
  rightPanel_.clear();
  leftPanel_.show();
  rightPanel_.show();

  initialized_ = true;
  return initGif();
}

bool NeopixelFaceDisplay::displayReady() const
{
  return initialized_;
}

void NeopixelFaceDisplay::drawPixel(int x, int y, Color color)
{
  if (!initialized_ || x < 0 || y < 0)
  {
    return;
  }

  const uint16_t panelX = static_cast<uint16_t>(x);
  const uint16_t panelY = static_cast<uint16_t>(y);

  if (panelY >= panelHeight_)
  {
    return;
  }

  const uint32_t pixelColor = Adafruit_NeoPixel::Color(color.getRed(), color.getGreen(), color.getBlue());

  if (panelX < panelWidth_)
  {
    leftPanel_.setPixelColor(getPixelIndex(panelX, panelY), pixelColor);
    return;
  }

  const uint16_t rightX = panelX - panelWidth_;
  if (rightX < panelWidth_)
  {
    rightPanel_.setPixelColor(getPixelIndex(rightX, panelY), pixelColor);
  }
}

void NeopixelFaceDisplay::afterFrameRendered()
{
  if (!initialized_)
  {
    return;
  }

  leftPanel_.show();
  rightPanel_.show();
}

uint16_t NeopixelFaceDisplay::getPixelIndex(uint16_t x, uint16_t y) const
{
  const uint16_t rowOffset = y * panelWidth_;

  if ((y & 1U) == 0)
  {
    return rowOffset + x;
  }

  return rowOffset + (panelWidth_ - 1U - x);
}
