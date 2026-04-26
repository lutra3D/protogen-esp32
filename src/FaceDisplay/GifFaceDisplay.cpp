#include "GifFaceDisplay.hpp"

GifFaceDisplay *GifFaceDisplay::instance_ = nullptr;

GifFaceDisplay::GifFaceDisplay()
    : gifFile_(),
      activeEmotionPath_(),
      isEmotionPlaying_(false)
{
  instance_ = this;
}

GifFaceDisplay::~GifFaceDisplay()
{
  closeEmotion();

  if (instance_ == this)
  {
    instance_ = nullptr;
  }
}


bool GifFaceDisplay::initGif()
{
  gif_.begin(LITTLE_ENDIAN_PIXELS);
  return true;
}

void GifFaceDisplay::playEmotion(const String &emotionPath)
{
  if (!displayReady())
  {
    return;
  }

  if (!isEmotionPlaying_ || emotionPath != activeEmotionPath_)
  {
    if (!openEmotion(emotionPath))
    {
      return;
    }
  }

  const int result = gif_.playFrame(true, nullptr);
  if (result >= 0)
  {
    afterFrameRendered();
    return;
  }

  Serial.printf("[E] GIF play error: %i\n", gif_.getLastError());
  if (!restartEmotion())
  {
    closeEmotion();
    Serial.printf("[E] Failed to continue GIF %s\n", emotionPath.c_str());
    return;
  }

  gif_.playFrame(true, nullptr);
  afterFrameRendered();
}

void GifFaceDisplay::afterFrameRendered()
{
}

void GifFaceDisplay::GIFDrawWrapper(GIFDRAW *pDraw)
{
  if (instance_ != nullptr)
  {
    instance_->GIFDraw(pDraw);
  }
}

void *GifFaceDisplay::fileOpenWrapper(const char *filename, int32_t *pFileSize)
{
  return instance_ != nullptr ? instance_->fileOpen(filename, pFileSize) : nullptr;
}

void GifFaceDisplay::fileCloseWrapper(void *pHandle)
{
  if (instance_ != nullptr)
  {
    instance_->fileClose(pHandle);
  }
}

int32_t GifFaceDisplay::fileReadWrapper(GIFFILE *pHandle, uint8_t *pBuf, int32_t iLen)
{
  return instance_ != nullptr ? instance_->fileRead(pHandle, pBuf, iLen) : 0;
}

int32_t GifFaceDisplay::fileSeekWrapper(GIFFILE *pHandle, int32_t iPosition)
{
  return instance_ != nullptr ? instance_->fileSeek(pHandle, iPosition) : -1;
}

void GifFaceDisplay::GIFDraw(GIFDRAW *pDraw)
{
  uint8_t *s;
  uint16_t *d, *usPalette, usTemp[320];
  int x, y;

  usPalette = pDraw->pPalette;
  y = pDraw->iY + pDraw->y;

  s = pDraw->pPixels;
  if (pDraw->ucDisposalMethod == 2)
  {
    for (x = 0; x < pDraw->iWidth; x++)
    {
      if (s[x] == pDraw->ucTransparent)
      {
        s[x] = pDraw->ucBackground;
      }
    }
    pDraw->ucHasTransparency = 0;
  }

  if (pDraw->ucHasTransparency)
  {
    uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
    int iCount = 0;
    pEnd = s + pDraw->iWidth;
    x = 0;
    while (x < pDraw->iWidth)
    {
      c = ucTransparent - 1;
      d = usTemp;
      while (c != ucTransparent && s < pEnd)
      {
        c = *s++;
        if (c == ucTransparent)
        {
          s--;
        }
        else
        {
          *d++ = usPalette[c];
          iCount++;
        }
      }
      if (iCount)
      {
        for (int xOffset = 0; xOffset < iCount; xOffset++)
        {
          drawPixel(x + xOffset + pDraw->iX, y, usTemp[xOffset]);
        }
        x += iCount;
        iCount = 0;
      }
      c = ucTransparent;
      while (c == ucTransparent && s < pEnd)
      {
        c = *s++;
        if (c == ucTransparent)
        {
          iCount++;
        }
        else
        {
          s--;
        }
      }
      if (iCount)
      {
        x += iCount;
        iCount = 0;
      }
    }
  }
  else
  {
    s = pDraw->pPixels;
    for (x = 0; x < pDraw->iWidth; x++)
    {
      drawPixel(x + pDraw->iX, y, usPalette[*s++]);
    }
  }
}

void *GifFaceDisplay::fileOpen(const char *filename, int32_t *pFileSize)
{
  gifFile_ = LittleFS.open(filename, FILE_READ);
  if (!gifFile_)
  {
    Serial.printf("Failed to open GIF file from LittleFS: %s\n", filename);
    *pFileSize = 0;
    return nullptr;
  }

  *pFileSize = gifFile_.size();
  Serial.printf("[I] Opened file: %s %d B\n", filename, gifFile_.size());
  return &gifFile_;
}

void GifFaceDisplay::fileClose(void *pHandle)
{
  (void)pHandle;
  if (gifFile_)
  {
    gifFile_.close();
  }
}

int32_t GifFaceDisplay::fileRead(GIFFILE *pHandle, uint8_t *pBuf, int32_t iLen)
{
  if (pHandle == nullptr || !pHandle->fHandle)
  {
    return 0;
  }

  File *file = static_cast<File *>(pHandle->fHandle);
  if (!(*file))
  {
    return 0;
  }

  int32_t bytesToRead = iLen;
  const int32_t remaining = pHandle->iSize - pHandle->iPos;
  if (remaining < bytesToRead)
  {
    bytesToRead = remaining;
  }

  if (bytesToRead <= 0)
  {
    return 0;
  }

  const int32_t bytesRead = file->read(pBuf, static_cast<size_t>(bytesToRead));
  pHandle->iPos = static_cast<int32_t>(file->position());
  return bytesRead;
}

int32_t GifFaceDisplay::fileSeek(GIFFILE *pHandle, int32_t iPosition)
{
  if (pHandle == nullptr || !pHandle->fHandle)
  {
    return -1;
  }

  File *file = static_cast<File *>(pHandle->fHandle);
  if (!(*file))
  {
    return -1;
  }

  if (iPosition < 0)
  {
    iPosition = 0;
  }

  if (iPosition > pHandle->iSize)
  {
    iPosition = pHandle->iSize;
  }

  if (!file->seek(iPosition, SeekSet))
  {
    return -1;
  }

  pHandle->iPos = static_cast<int32_t>(file->position());
  return pHandle->iPos;
}

bool GifFaceDisplay::openEmotion(const String &emotionPath, bool logTransition)
{
  if (emotionPath.isEmpty())
  {
    Serial.println(F("[E] Emotion path is empty"));
    closeEmotion();
    return false;
  }

  closeEmotion();

  if (!gif_.open(emotionPath.c_str(), fileOpenWrapper, fileCloseWrapper, fileReadWrapper,
                 fileSeekWrapper, GIFDrawWrapper))
  {
    Serial.printf("[E] Failed to open GIF %s\n", emotionPath.c_str());
    return false;
  }

  activeEmotionPath_ = emotionPath;
  isEmotionPlaying_ = true;

  if (logTransition)
  {
    Serial.printf("[I] Playing GIF %s\n", emotionPath.c_str());
  }

  return true;
}

void GifFaceDisplay::closeEmotion()
{
  if (isEmotionPlaying_)
  {
    gif_.close();
  }

  if (gifFile_)
  {
    gifFile_.close();
  }

  isEmotionPlaying_ = false;
  activeEmotionPath_ = "";
}

bool GifFaceDisplay::restartEmotion()
{
  if (activeEmotionPath_.isEmpty())
  {
    return false;
  }

  const String path = activeEmotionPath_;
  closeEmotion();
  return openEmotion(path, false);
}
