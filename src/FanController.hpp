#ifndef FAN_CONTROLLER_HPP
#define FAN_CONTROLLER_HPP

#include <Arduino.h>

class FanController {
public:
  FanController(uint8_t pwmPin, uint8_t channel, uint32_t frequency, uint8_t resolution);

  void begin();
  bool setDutyCycle(int dutyCycle);
  int getDutyCycle() const;
  int getMaxDutyCycle() const;

private:
  uint8_t pwmPin_;
  uint8_t channel_;
  uint32_t frequency_;
  uint8_t resolution_;
  int dutyCycle_;
};

#endif // FAN_CONTROLLER_HPP
