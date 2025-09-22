#include "EarController.hpp"

EarController::EarController(uint16_t ledCount, uint8_t dataPin)
    : ledCount_(ledCount),
      earLeds_(ledCount, dataPin, NEO_GRB + NEO_KHZ800),
      brightness_(80),
      red_(255),
      green_(255),
      blue_(255) {}

bool EarController::begin() {
  earLeds_.begin();
  earLeds_.setBrightness(brightness_);
  return true;
}

void EarController::setBrightness(uint8_t brightness) {
  brightness_ = brightness;
}

uint8_t EarController::getBrightness() const {
  return brightness_;
}

void EarController::setColor(uint8_t red, uint8_t green, uint8_t blue) {
  red_ = red;
  green_ = green;
  blue_ = blue;
}

uint8_t EarController::getRed() const {
  return red_;
}

uint8_t EarController::getGreen() const {
  return green_;
}

uint8_t EarController::getBlue() const {
  return blue_;
}

String EarController::getColorHexString() const {
  String result = "#";
  result += String(red_, 16);
  result += String(green_, 16);
  result += String(blue_, 16);
  result += " ";
  result += String(brightness_);
  return result;
}

void EarController::update() {
  earLeds_.setBrightness(brightness_);
  for (uint16_t index = 0; index < ledCount_; ++index) {
    earLeds_.setPixelColor(index, Adafruit_NeoPixel::Color(red_, green_, blue_));
  }
  earLeds_.show();
}
