#ifndef FACE_DISPLAY_HPP
#define FACE_DISPLAY_HPP

#include <AnimatedGIF.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <LittleFS.h>

#include <Arduino.h>
#include <Graphics/Color.hpp>

class GifFaceDisplay {
public:
  virtual ~GifFaceDisplay();
  virtual bool begin() = 0;
  virtual bool displayReady() const = 0;


  void playEmotion(const String &emotionPath);

  protected:
  GifFaceDisplay();

  virtual void drawPixel(int x, int y, Color color) = 0;

  bool initGif();

  void GIFDraw(GIFDRAW *pDraw);
  void *fileOpen(const char *filename, int32_t *pFileSize);
  void fileClose(void *pHandle);
  int32_t fileRead(GIFFILE *pHandle, uint8_t *pBuf, int32_t iLen);
  int32_t fileSeek(GIFFILE *pHandle, int32_t iPosition);
  bool openEmotion(const String &emotionPath, bool logTransition = true);
  void closeEmotion();
  bool restartEmotion();
  void initializeColors();

  static void GIFDrawWrapper(GIFDRAW *pDraw);
  static void *fileOpenWrapper(const char *filename, int32_t *pFileSize);
  static void fileCloseWrapper(void *pHandle);
  static int32_t fileReadWrapper(GIFFILE *pHandle, uint8_t *pBuf, int32_t iLen);
  static int32_t fileSeekWrapper(GIFFILE *pHandle, int32_t iPosition);

  static GifFaceDisplay *instance_;

  AnimatedGIF gif_;
  File gifFile_;
  String activeEmotionPath_;
  bool isEmotionPlaying_;
};

#endif // FACE_DISPLAY_HPP
