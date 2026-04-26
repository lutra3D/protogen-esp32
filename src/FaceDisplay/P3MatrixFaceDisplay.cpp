#include "P3MatrixFaceDisplay.hpp"

P3MatrixFaceDisplay::P3MatrixFaceDisplay(int panelResX, int panelResY, int panelChainLength)
    : GifFaceDisplay(),
      display_(nullptr),
      colorBlue_(0),
      colorBlack_(0)
{
    if (display_ != nullptr)
    {
        delete display_;
        display_ = nullptr;
    }
}

P3MatrixFaceDisplay::~P3MatrixFaceDisplay()
{
    closeEmotion();

    if (display_ != nullptr)
    {
        delete display_;
        display_ = nullptr;
    }

    if (instance_ == this)
    {
        instance_ = nullptr;
    }
}

bool P3MatrixFaceDisplay::begin()
{
  HUB75_I2S_CFG mxconfig(panelResX_, panelResY_, panelChainLength_);
  mxconfig.gpio.e = 18;
  mxconfig.clkphase = false;

  display_ = new MatrixPanel_I2S_DMA(mxconfig);
  display_->begin();

  initializeColors();

  display_->fillScreen(colorBlack_);
  display_->setCursor(5, 0);
  display_->setTextColor(colorBlue_);
  display_->setTextSize(1);
  display_->println("ProtoFW 2.0");
  display_->setCursor(12, 10);
  display_->println("Lutra 3D");

  initGif();
}

void P3MatrixFaceDisplay::drawPixel(int x, int y, Color color)
{
  display_->drawPixel(x, y, display_->color565(color.getRed(), color.getGreen(), color.getBlue()));
}

void P3MatrixFaceDisplay::initializeColors()
{
  colorBlue_ = display_->color565(0, 0, 255);
  colorBlack_ = display_->color565(0, 0, 0);
}