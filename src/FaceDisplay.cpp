#include "FaceDisplay.hpp"

FaceDisplay::FaceDisplay(int panelResX, int panelResY, int panelChainLength)
    : AbstractFaceDisplay(panelResX * panelChainLength, panelResY),
      panelChainLength_(panelChainLength),
      display_(nullptr),
      colorBlue_(0),
      colorBlack_(0)
{
}

FaceDisplay::~FaceDisplay()
{
  if (display_ != nullptr)
  {
    delete display_;
    display_ = nullptr;
  }
}

bool FaceDisplay::initializeDisplay()
{
  const int panelResX = width_ / panelChainLength_;
  HUB75_I2S_CFG mxconfig(panelResX, height_, panelChainLength_);
  mxconfig.gpio.e = 18;
  mxconfig.clkphase = false;

  display_ = new MatrixPanel_I2S_DMA(mxconfig);
  display_->begin();

  initializeColors();
  return true;
}

void FaceDisplay::drawPixel(int x, int y, uint16_t color565)
{
  if (display_ != nullptr)
  {
    display_->drawPixel(x, y, color565);
  }
}

void FaceDisplay::clearFrame()
{
  if (display_ != nullptr)
  {
    display_->fillScreen(colorBlack_);
  }
}

void FaceDisplay::presentFrame()
{
}

void FaceDisplay::showBootScreen()
{
  if (display_ == nullptr)
  {
    return;
  }

  display_->fillScreen(colorBlack_);
  display_->setCursor(5, 0);
  display_->setTextColor(colorBlue_);
  display_->setTextSize(1);
  display_->println("ProtoFW 2.0");
  display_->setCursor(12, 10);
  display_->println("Lutra 3D");
}

void FaceDisplay::initializeColors()
{
  colorBlue_ = display_->color565(0, 0, 255);
  colorBlack_ = display_->color565(0, 0, 0);
}
