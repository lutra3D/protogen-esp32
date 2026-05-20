#include "LedBrightnessController.hpp"

LedBrightnessController::LedBrightnessController() : ledBrightness_() {}
void LedBrightnessController::setBrightness(uint8_t brightness) { ledBrightness_.setBrightness(brightness); }
void LedBrightnessController::setBrightnessPercent(float percent) { ledBrightness_.setBrightnessPercent(percent); }
uint8_t LedBrightnessController::getBrightness() const { return ledBrightness_.getBrightness(); }
float LedBrightnessController::getBrightnessPercent() const { return ledBrightness_.getBrightnessPercent(); }
LedBrightness &LedBrightnessController::getLedBrightness() { return ledBrightness_; }
const LedBrightness &LedBrightnessController::getLedBrightness() const { return ledBrightness_; }
