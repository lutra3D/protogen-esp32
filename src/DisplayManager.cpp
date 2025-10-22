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
  display_.clearDisplay();
  display_.setCursor(0, 0);

  display_.print(F("Anim:"));
  display_.println(emotionState_.getCurrentEmotion());

  display_.print(F("Fan:"));
  display_.print(fanController_.getDutyCyclePercent(), 0);
  display_.println(F("%"));

  display_.println(formatEarInfo());

  IPAddress ip = WiFi.softAPIP();
  display_.print(F("IP:"));
  display_.println(ip.toString());
}

String DisplayManager::formatEarInfo() const {
  String line = F("Ear:#");
  line += FormatHex(earController_.getRed());
  line += FormatHex(earController_.getGreen());
  line += FormatHex(earController_.getBlue());
  line += F(" B:");

  float brightnessPercent =
      (static_cast<float>(earController_.getBrightness()) / 255.0f) * 100.0f;
  line += String(static_cast<int>(brightnessPercent + 0.5f));
  line += F("%");
  return line;
}
