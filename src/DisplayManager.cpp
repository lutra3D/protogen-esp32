#include "DisplayManager.hpp"

#include <WiFi.h>
#include <Wire.h>

namespace {
constexpr uint8_t kDisplayAddress = 0x3C;
constexpr uint8_t kRotation = 2;

String FormatHex(uint8_t value) {
  String text = String(value, HEX);
  if (text.length() < 2) {
    text = "0" + text;
  }
  text.toUpperCase();
  return text;
}

const uint8_t FAN_ICON_12X12[] PROGMEM = {
  0x7f, 0xe0, 0xa1, 0x50, 0xe3, 0x30, 0xb6, 0x10, 
  0x9f, 0x70, 0x8f, 0xd0, 0xbf, 0x10, 0xaf, 0x90, 
	0xe6, 0xd0, 0xcc, 0x70, 0xa8, 0x50, 0x7f, 0xe0
};

const uint8_t WIFI_ICON_12X12[] PROGMEM = {
  0x00, 0x00, 0x06, 0x00, 0x1f, 0x80, 0x20, 0x40,
  0x4f, 0x20, 0x10, 0x80, 0x26, 0x40, 0x09, 0x00, 
  0x06, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t EAR_ICON_12X12[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x83, 0xc0,
  0xa2, 0x20, 0x4c, 0x10, 0x24, 0x10, 0x14, 0x10, 
	0x0c, 0x10, 0x06, 0x20, 0x03, 0xc0, 0x00, 0x00
};
} // namespace

DisplayManager::DisplayManager(uint8_t sdaPin, uint8_t sclPin,
                               EmotionState &emotionState,
                               FanController &fanController,
                               EarController &earController)
    : sdaPin_(sdaPin),
      sclPin_(sclPin),
      emotionState_(emotionState),
      fanController_(fanController),
      earController_(earController),
      display_() {}

void DisplayManager::begin() {
  Wire.begin(sdaPin_, sclPin_);
  display_.begin(SH1106_EXTERNALVCC, kDisplayAddress);

  display_.clearDisplay();
  display_.setTextSize(1);
  display_.setTextColor(WHITE);
  display_.setRotation(kRotation);
  display_.display();
}

void DisplayManager::update() {
  renderStatus();
  display_.display();
}

void DisplayManager::renderStatus() {
  const uint8_t lineHeight = 14;
  
  display_.clearDisplay();
  display_.setCursor(0, 0);

  display_.setTextSize(2);
  display_.println(emotionState_.getDisplayEmotion());
  
  display_.setTextSize(1);
  DrawIconLine(FAN_ICON_12X12, 18, formatFanInfo());
  DrawIconLine(EAR_ICON_12X12, 18+lineHeight, formatEarInfo());

  IPAddress ip = WiFi.softAPIP();
  DrawIconLine(WIFI_ICON_12X12, 18+2*lineHeight, ip.toString());
}

void DisplayManager::DrawIconLine(const uint8_t* icon, uint8_t offsetTop, String text){
  const uint8_t lineOffsetPx = 2;
  const uint8_t iconSize = 12;
  const uint8_t textOffsetLeft = 20;

  display_.drawBitmap(0,offsetTop, icon, iconSize, iconSize, WHITE);
  display_.setCursor(textOffsetLeft,offsetTop+lineOffsetPx);
  display_.print(text);
}

String DisplayManager::formatEarInfo() const {
  String line = F("");
  line += earController_.getColorHexString();
  line += F(" B:");
  line += String(static_cast<int>(earController_.getBrightnessPercent() + 0.5f));
  line += F("%");
  return line;
}

String DisplayManager::formatFanInfo() const {
  String line = F("");
  line += String(static_cast<int>(fanController_.getDutyCyclePercent()));
  line += F("%");
  return line;
}
