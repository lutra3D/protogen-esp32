#ifndef ABSTRACT_FACE_DISPLAY_HPP
#define ABSTRACT_FACE_DISPLAY_HPP

#include <AnimatedGIF.h>
#include <LittleFS.h>

#include <Arduino.h>

class AbstractFaceDisplay {
public:
  virtual ~AbstractFaceDisplay();

  bool begin();
  void playEmotion(const String &emotionPath);

protected:
  AbstractFaceDisplay(int width, int height);

  virtual bool initializeDisplay() = 0;
  virtual void drawPixel(int x, int y, uint16_t color565) = 0;
  virtual void clearFrame() = 0;
  virtual void presentFrame() = 0;
  virtual void showBootScreen() = 0;

  int width_;
  int height_;

private:
  static AbstractFaceDisplay *instance_;

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

  AnimatedGIF gif_;
  File gifFile_;
  String activeEmotionPath_;
  bool isEmotionPlaying_;
};

#endif // ABSTRACT_FACE_DISPLAY_HPP
