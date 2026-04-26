#ifndef P3MATRIXFACEDISPLAY_HPP
#define P3MATRIXFACEDISPLAY_HPP

#include "GifFaceDisplay.hpp"

class P3MatrixFaceDisplay : public GifFaceDisplay
{
public:
  P3MatrixFaceDisplay(int panelResX, int panelResY, int panelChainLength);
  ~P3MatrixFaceDisplay() override;
  bool begin() override;
  void drawPixel(int x, int y, Color color) override;
  bool displayReady() const override;

private:
  void initializeColors();

  int panelResX_;
  int panelResY_;
  int panelChainLength_;

  MatrixPanel_I2S_DMA *display_;
  
  uint16_t colorBlue_;
  uint16_t colorBlack_;
};

#endif // P3MATRIXFACEDISPLAY_HPP