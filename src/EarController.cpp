#include "EarController.hpp"

#include <math.h>

EarController::EarController(uint16_t ledCount, uint8_t dataPin)
    : ledCount_(ledCount),
      earLeds_(ledCount, dataPin, NEO_GRB + NEO_KHZ800),
      ear_(),
      display_()
      { }

bool EarController::begin() {
  earLeds_.begin();
  earLeds_.setBrightness(ear_.getBrightness());
  return true;
}

void EarController::setBrightness(uint8_t brightness) {
  ear_.setBrightness(brightness);
}

void EarController::setBrightnessPercent(float percent) {
  ear_.setBrightnessPercent(percent);
}

uint8_t EarController::getBrightness() const {
  return ear_.getBrightness();
}

void EarController::setColor(Color color) {
  ear_.setColor(color);
}

void EarController::setGradient(Gradient gradient) {
  ear_.setGradient(gradient);
}


String EarController::getColorHexString() const {
  return ear_.getColorHexString();
}

float EarController::getBrightnessPercent() const {
  return ear_.getBrightnessPercent();
}

Ear &EarController::getEar() {
  return ear_;
}

void EarController::update() {
  earLeds_.setBrightness(ear_.getBrightness());

  if (ear_.getColorMode() == ColorMode::Gradient)
  {
    const auto &gradient = ear_.getGradient();
    for (uint16_t index = 0; index < ledCount_; ++index)
    {
      auto color = gradient.rasterize(index, ledCount_, display_);
      earLeds_.setPixelColor(index, Adafruit_NeoPixel::Color(color.getRed(), color.getGreen(), color.getBlue()));
    }
  }
  else
  {
    const auto &color = ear_.getColor();
    for (uint16_t index = 0; index < ledCount_; ++index)
    {
      earLeds_.setPixelColor(index, Adafruit_NeoPixel::Color(color.getRed(), color.getGreen(), color.getBlue()));
    }
  }

  earLeds_.show();
}
