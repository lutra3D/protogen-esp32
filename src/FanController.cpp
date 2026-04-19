#include "FanController.hpp"

FanController::FanController(uint8_t pwmPin, uint8_t channel, uint32_t frequency,
                             uint8_t resolution)
    : pwmPin_(pwmPin),
      channel_(channel),
      frequency_(frequency),
      resolution_(resolution),
      dutyCycle_(100) {}

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



bool FanController::setDutyCyclePercent(float percent) {
  if (percent < 0.0f || percent > 100.0f) {
    return false;
  }

  const int maxValue = getMaxDutyCycle();
  const int dutyCycle = static_cast<int>((percent / 100.0f) * static_cast<float>(maxValue));
  return setDutyCycle(dutyCycle);
}

int FanController::getDutyCycle() const { return dutyCycle_; }

int FanController::getMaxDutyCycle() const { return (1 << resolution_) - 1; }

float FanController::getDutyCyclePercent() const {
  const int maxValue = getMaxDutyCycle();
  if (maxValue <= 0) {
    return 0.0f;
  }
  return (static_cast<float>(dutyCycle_) / static_cast<float>(maxValue)) * 100.0f;
}
