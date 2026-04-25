#ifndef FACE_DISPLAY_HPP
#define FACE_DISPLAY_HPP

#include <AnimatedGIF.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <LittleFS.h>

#include <Arduino.h>

class FaceDisplay {
public:
  FaceDisplay(int panelResX, int panelResY, int panelChainLength);
  ~FaceDisplay();

  bool begin();
  void playEmotion(const String &emotionPath);

private:
  static FaceDisplay *instance_;

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
  bool openEmotion(const String &emotionPath, bool logTransition = true);
  void closeEmotion();
  bool restartEmotion();
  void initializeColors();

  int panelResX_;
  int panelResY_;
  int panelChainLength_;

  MatrixPanel_I2S_DMA *display_;
  AnimatedGIF gif_;
  File gifFile_;
  String activeEmotionPath_;
  bool isEmotionPlaying_;

  uint16_t colorBlue_;
  uint16_t colorBlack_;
};

#endif // FACE_DISPLAY_HPP
