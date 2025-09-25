#include "AnimationManager.hpp"

AnimationManager *AnimationManager::instance_ = nullptr;

AnimationManager::AnimationManager(int panelResX, int panelResY, int panelChainLength)
    : panelResX_(panelResX),
      panelResY_(panelResY),
      panelChainLength_(panelChainLength),
      display_(nullptr),
      gifFile_(),
      colorRed_(0),
      colorGreen_(0),
      colorBlue_(0),
      colorWhite_(0),
      colorYellow_(0),
      colorCyan_(0),
      colorMagenta_(0),
      colorBlack_(0) {
  instance_ = this;
}

AnimationManager::~AnimationManager() {
  if (display_ != nullptr) {
    delete display_;
    display_ = nullptr;
  }
  if (instance_ == this) {
    instance_ = nullptr;
  }
}

bool AnimationManager::begin() {
  HUB75_I2S_CFG mxconfig(panelResX_, panelResY_, panelChainLength_);
  mxconfig.gpio.e = 18;
  mxconfig.clkphase = false;

  display_ = new MatrixPanel_I2S_DMA(mxconfig);
  display_->begin();

  initializeColors();

  display_->fillScreen(colorBlack_);
  display_->setCursor(5, 0);
  display_->setTextColor(colorBlue_);
  display_->println("Startup...");

  if (!SPIFFS.begin(true)) {
    Serial.println(F("SPIFFS mount failed!"));
    return false;
  }

  gif_.begin(LITTLE_ENDIAN_PIXELS);

  listSPIFFS();
  return true;
}

void AnimationManager::playEmotion(const String &emotionPath) {
  if (!display_) {
    return;
  }

  if (gif_.open(emotionPath.c_str(), fileOpenWrapper, fileCloseWrapper, fileReadWrapper, fileSeekWrapper, GIFDrawWrapper)) {
    gif_.playFrame(true, nullptr);
    gif_.close();
  }
}

void AnimationManager::listSPIFFS() {
  Serial.println(F("SPIFFS contents:"));
  File root = SPIFFS.open("/");
  if (!root) {
    Serial.println(F("Failed to open root directory"));
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(F("Root is not a directory"));
    return;
  }

  File file = root.openNextFile();
  while (file) {
    Serial.printf("  %s  (%d bytes)\n", file.name(), file.size());
    file = root.openNextFile();
  }
  Serial.println(F("---- End of SPIFFS listing ----"));
}

void AnimationManager::GIFDrawWrapper(GIFDRAW *pDraw) {
  if (instance_) {
    instance_->GIFDraw(pDraw);
  }
}

void *AnimationManager::fileOpenWrapper(const char *filename, int32_t *pFileSize) {
  return instance_ ? instance_->fileOpen(filename, pFileSize) : nullptr;
}

void AnimationManager::fileCloseWrapper(void *pHandle) {
  if (instance_) {
    instance_->fileClose(pHandle);
  }
}

int32_t AnimationManager::fileReadWrapper(GIFFILE *pHandle, uint8_t *pBuf, int32_t iLen) {
  return instance_ ? instance_->fileRead(pHandle, pBuf, iLen) : 0;
}

int32_t AnimationManager::fileSeekWrapper(GIFFILE *pHandle, int32_t iPosition) {
  return instance_ ? instance_->fileSeek(pHandle, iPosition) : -1;
}

void AnimationManager::GIFDraw(GIFDRAW *pDraw) {
  uint8_t *s;
  uint16_t *d, *usPalette, usTemp[320];
  int x, y;

  usPalette = pDraw->pPalette;
  y = pDraw->iY + pDraw->y;

  s = pDraw->pPixels;
  if (pDraw->ucDisposalMethod == 2) {
    for (x = 0; x < pDraw->iWidth; x++) {
      if (s[x] == pDraw->ucTransparent) {
        s[x] = pDraw->ucBackground;
      }
    }
    pDraw->ucHasTransparency = 0;
  }

  if (pDraw->ucHasTransparency) {
    uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
    int iCount = 0;
    pEnd = s + pDraw->iWidth;
    x = 0;
    while (x < pDraw->iWidth) {
      c = ucTransparent - 1;
      d = usTemp;
      while (c != ucTransparent && s < pEnd) {
        c = *s++;
        if (c == ucTransparent) {
          s--;
        } else {
          *d++ = usPalette[c];
          iCount++;
        }
      }
      if (iCount) {
        for (int xOffset = 0; xOffset < iCount; xOffset++) {
          display_->drawPixel(x + xOffset + pDraw->iX, y, usTemp[xOffset]);
        }
        x += iCount;
        iCount = 0;
      }
      c = ucTransparent;
      while (c == ucTransparent && s < pEnd) {
        c = *s++;
        if (c == ucTransparent) {
          iCount++;
        } else {
          s--;
        }
      }
      if (iCount) {
        x += iCount;
        iCount = 0;
      }
    }
  } else {
    s = pDraw->pPixels;
    for (x = 0; x < pDraw->iWidth; x++) {
      display_->drawPixel(x + pDraw->iX, y, usPalette[*s++]);
    }
  }
}

void *AnimationManager::fileOpen(const char *filename, int32_t *pFileSize) {
  gifFile_ = SPIFFS.open(filename, FILE_READ);
  if (!gifFile_) {
    Serial.printf("Failed to open GIF file from SPIFFS: %s\n", filename);
    *pFileSize = 0;
    return nullptr;
  }
  *pFileSize = gifFile_.size();
  return &gifFile_;
}

void AnimationManager::fileClose(void *pHandle) {
  (void)pHandle;
  if (gifFile_) {
    gifFile_.close();
  }
}

int32_t AnimationManager::fileRead(GIFFILE *pHandle, uint8_t *pBuf, int32_t iLen) {
  (void)pHandle;
  if (!gifFile_) {
    return 0;
  }
  return gifFile_.read(pBuf, static_cast<size_t>(iLen));
}

int32_t AnimationManager::fileSeek(GIFFILE *pHandle, int32_t iPosition) {
  (void)pHandle;
  if (!gifFile_) {
    return -1;
  }
  if (iPosition < 0) {
    iPosition = 0;
  }
  if (iPosition > static_cast<int32_t>(gifFile_.size())) {
    iPosition = gifFile_.size();
  }
  gifFile_.seek(iPosition, SeekSet);
  return iPosition;
}

void AnimationManager::initializeColors() {
  colorRed_ = display_->color565(255, 0, 0);
  colorGreen_ = display_->color565(0, 255, 0);
  colorBlue_ = display_->color565(0, 0, 255);
  colorWhite_ = display_->color565(255, 255, 255);
  colorYellow_ = display_->color565(255, 255, 0);
  colorCyan_ = display_->color565(0, 255, 255);
  colorMagenta_ = display_->color565(255, 0, 255);
  colorBlack_ = display_->color565(0, 0, 0);
}
