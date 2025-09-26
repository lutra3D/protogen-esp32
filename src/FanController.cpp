#include "FanController.hpp"

FanController::FanController(uint8_t pwmPin, uint8_t channel, uint32_t frequency, uint8_t resolution)
    : pwmPin_(pwmPin), channel_(channel), frequency_(frequency), resolution_(resolution), dutyCycle_(100) {}

void FanController::begin() {
  pinMode(pwmPin_, OUTPUT);
  ledcSetup(channel_, frequency_, resolution_);
  ledcAttachPin(pwmPin_, channel_);
  ledcWrite(channel_, dutyCycle_);
}

bool FanController::setDutyCycle(int dutyCycle) {
  if (dutyCycle < 0 || dutyCycle >= (1 << resolution_)) {
    return false;
  }
  dutyCycle_ = dutyCycle;
  ledcWrite(channel_, dutyCycle_);
  return true;
}

int FanController::getDutyCycle() const {
  return dutyCycle_;
}

int FanController::getMaxDutyCycle() const {
  return (1 << resolution_) - 1;
}
