#ifndef ANIMATION_MANAGER_HPP
#define ANIMATION_MANAGER_HPP

#include <AnimatedGIF.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <SPIFFS.h>

#include <Arduino.h>

#include "float_helper.hpp"

struct AnimationInfo {
    String name; 
    String path;
};

class AnimationManager {
public:
  AnimationManager(int panelResX, int panelResY, int panelChainLength);
  ~AnimationManager();

  bool begin();
  void playEmotion(const String &emotionPath);
  std::vector<AnimationInfo> getEmotions();
  void printEmotions();

private:
  static AnimationManager *instance_;

  static void GIFDrawWrapper(GIFDRAW *pDraw);
  static void *fileOpenWrapper(const char *filename, int32_t *pFileSize);
  static void fileCloseWrapper(void *pHandle);
  static int32_t fileReadWrapper(GIFFILE *pHandle, uint8_t *pBuf, int32_t iLen);
  static int32_t fileSeekWrapper(GIFFILE *pHandle, int32_t iPosition);

  void GIFDraw(GIFDRAW *pDraw);
  void *fileOpen(const char *filename, int32_t *pFileSize);
  void fileClose(void *pHandle);
  int32_t fileRead(GIFFILE *pHandle, uint8_t *pBuf, int32_t iLen);
  int32_t fileSeek(GIFFILE *pHandle, int32_t iPosition);
  void initializeColors();

  int panelResX_;
  int panelResY_;
  int panelChainLength_;

  MatrixPanel_I2S_DMA *display_;
  AnimatedGIF gif_;
  File gifFile_;

  uint16_t colorRed_;
  uint16_t colorGreen_;
  uint16_t colorBlue_;
  uint16_t colorWhite_;
  uint16_t colorYellow_;
  uint16_t colorCyan_;
  uint16_t colorMagenta_;
  uint16_t colorBlack_;
};

#endif // ANIMATION_MANAGER_HPP
