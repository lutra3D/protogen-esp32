#ifndef FACE_DISPLAY_HPP
#define FACE_DISPLAY_HPP

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

#include "AbstractFaceDisplay.hpp"

class FaceDisplay : public AbstractFaceDisplay {
public:
  FaceDisplay(int panelResX, int panelResY, int panelChainLength);
  ~FaceDisplay() override;

protected:
  bool initializeDisplay() override;
  void drawPixel(int x, int y, uint16_t color565) override;
  void clearFrame() override;
  void presentFrame() override;
  void showBootScreen() override;

private:
  void initializeColors();

  int panelChainLength_;
  MatrixPanel_I2S_DMA *display_;

  uint16_t colorBlue_;
  uint16_t colorBlack_;
};

#endif // FACE_DISPLAY_HPP
