#include "EarController.hpp"

EarController::EarController(uint16_t ledCount, uint8_t dataPin, LedBrightnessController &brightnessController)
    : ledCount_(ledCount),
      earLeds_(ledCount, dataPin, NEO_GRB + NEO_KHZ800),
      ear_(),
      display_(ledCount, 61.0f),
      brightnessController_(brightnessController)
      { }

bool EarController::begin() {
  earLeds_.begin();
  earLeds_.setBrightness(brightnessController_.getBrightness());
  return true;
}

void EarController::setColor(Color color) { ear_.setColor(color); }
void EarController::setGradient(Gradient gradient) { ear_.setGradient(gradient); }
Ear &EarController::getEar() { return ear_; }

void EarController::update() {
  earLeds_.setBrightness(brightnessController_.getBrightness());
  if (ear_.getColorMode() == ColorMode::Gradient) {
    const auto &gradient = ear_.getGradient();
    for (uint16_t index = 0; index < ledCount_; ++index) {
      auto color = gradient.rasterize(index, ledCount_, display_);
      earLeds_.setPixelColor(index, Adafruit_NeoPixel::gamma32(Adafruit_NeoPixel::Color(color.getRed(), color.getGreen(), color.getBlue())));
    }
  } else {
    const auto &color = ear_.getColor();
    for (uint16_t index = 0; index < ledCount_; ++index) {
      earLeds_.setPixelColor(index, Adafruit_NeoPixel::gamma32(Adafruit_NeoPixel::Color(color.getRed(), color.getGreen(), color.getBlue())));
    }
  }
  earLeds_.show();
}
