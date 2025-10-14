#include "StatusDisplay.hpp"

#include "EarController.hpp"
#include "EmotionState.hpp"
#include "FanController.hpp"

#include <WiFi.h>
#include <Wire.h>
#include <stdio.h>

namespace {

// Simple 16x16 monochrome icons stored in program memory to minimize RAM use.
const uint8_t FAN_ICON_16X16[] PROGMEM = {
    0x00, 0x00, 0x07, 0xE0, 0x1F, 0xF8, 0x3C, 0x3C,
    0x78, 0x1E, 0x70, 0x0E, 0xE3, 0x37, 0xC7, 0xF3,
    0xCD, 0x87, 0xE6, 0x0F, 0x70, 0x0E, 0x78, 0x1E,
    0x3C, 0x3C, 0x1F, 0xF8, 0x07, 0xE0, 0x00, 0x00,
};

const uint8_t WIFI_ICON_16X16[] PROGMEM = {
    0x07, 0xE0, 0x1C, 0x38, 0x30, 0x0C, 0x00, 0x00,
    0x03, 0xC0, 0x06, 0x30, 0x00, 0x00, 0x00, 0xC0,
    0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0xC0,
    0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

constexpr int kIconWidth = 16;
constexpr int kIconHeight = 16;
constexpr int kTextIndent = kIconWidth + 2;

} // namespace

StatusDisplay::StatusDisplay(EmotionState &emotionState,
                             FanController &fanController,
                             EarController &earController,
                             int8_t resetPin,
                             uint8_t i2cAddress)
    : emotionState_(emotionState),
      fanController_(fanController),
      earController_(earController),
      display_(resetPin),
      i2cAddress_(i2cAddress),
      lastUpdateMs_(0),
      updateIntervalMs_(1000),
      lastEmotion_(),
      lastDutyCycle_(-1),
      lastRed_(0),
      lastGreen_(0),
      lastBlue_(0),
      lastBrightness_(0),
      lastApIp_() {}

bool StatusDisplay::begin() {
  Wire.begin();

  display_.begin(SH1106_SWITCHCAPVCC, i2cAddress_);

  display_.clearDisplay();
  display_.display();

  drawStatus(true);
  return true;
}

void StatusDisplay::update() {
  const uint32_t now = millis();
  if (now - lastUpdateMs_ < updateIntervalMs_) {
    return;
  }
  lastUpdateMs_ = now;
  drawStatus();
}

void StatusDisplay::setUpdateInterval(uint32_t intervalMs) {
  updateIntervalMs_ = intervalMs;
}

void StatusDisplay::drawStatus(bool force) {
  const String &currentEmotion = emotionState_.getCurrentEmotion();
  const int currentDutyCycle = fanController_.getDutyCycle();
  const uint8_t currentRed = earController_.getRed();
  const uint8_t currentGreen = earController_.getGreen();
  const uint8_t currentBlue = earController_.getBlue();
  const uint8_t currentBrightness = earController_.getBrightness();
  const String currentApIp = getAccessPointIp();

  if (!force && currentEmotion == lastEmotion_ && currentDutyCycle == lastDutyCycle_ &&
      currentRed == lastRed_ && currentGreen == lastGreen_ && currentBlue == lastBlue_ &&
      currentBrightness == lastBrightness_ && currentApIp == lastApIp_) {
    return;
  }

  display_.clearDisplay();
  display_.setTextSize(1);
  display_.setTextColor(WHITE);

  display_.setCursor(kTextIndent, 0);
  display_.print(F("Anim: "));
  display_.println(currentEmotion);

  display_.drawBitmap(0, 12, FAN_ICON_16X16, kIconWidth, kIconHeight, WHITE);
  display_.setCursor(kTextIndent, 12);
  display_.print(F("Fan: "));
  display_.print(formatFanSpeedPercent(currentDutyCycle));
  display_.println(F("%"));

  display_.setCursor(0, 28);
  display_.print(F("Color: "));
  display_.println(formatEarColor(currentRed, currentGreen, currentBlue));

  display_.setCursor(0, 40);
  display_.print(F("Bright: "));
  display_.println(currentBrightness);

  display_.drawBitmap(0, 48, WIFI_ICON_16X16, kIconWidth, kIconHeight, WHITE);
  display_.setCursor(kTextIndent, 48);
  display_.print(F("WiFi: "));
  display_.println(currentApIp);

  display_.display();

  lastEmotion_ = currentEmotion;
  lastDutyCycle_ = currentDutyCycle;
  lastRed_ = currentRed;
  lastGreen_ = currentGreen;
  lastBlue_ = currentBlue;
  lastBrightness_ = currentBrightness;
  lastApIp_ = currentApIp;
}

String StatusDisplay::formatFanSpeedPercent(int dutyCycle) const {
  const int maxDutyCycle = fanController_.getMaxDutyCycle();
  if (maxDutyCycle <= 0) {
    return String("0");
  }

  const float percent = (static_cast<float>(dutyCycle) / static_cast<float>(maxDutyCycle)) * 100.0f;
  const int roundedPercent = static_cast<int>(percent + 0.5f);

  return String(roundedPercent);
}

String StatusDisplay::formatEarColor(uint8_t red, uint8_t green, uint8_t blue) const {
  char buffer[8];
  snprintf(buffer, sizeof(buffer), "#%02X%02X%02X", red, green, blue);
  return String(buffer);
}

String StatusDisplay::getAccessPointIp() const {
  IPAddress apIp = WiFi.softAPIP();
  if (!apIp) {
    return String(F("0.0.0.0"));
  }
  return apIp.toString();
}
