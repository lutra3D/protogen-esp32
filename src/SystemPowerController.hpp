#ifndef SYSTEM_POWER_CONTROLLER_HPP
#define SYSTEM_POWER_CONTROLLER_HPP

#include <Arduino.h>
#include <Wire.h>

class SystemPowerController {
public:
  explicit SystemPowerController(uint8_t sdaPin, uint8_t sclPin);

  bool begin();
  bool isEnabled() const;
  String readPowerInfo();

private:
  bool readRegister16(uint8_t reg, uint16_t &value) const;

  uint8_t sdaPin_;
  uint8_t sclPin_;
  bool enabled_;

  static constexpr uint8_t kIna226Address = 0x40;
  static constexpr uint8_t kBusVoltageRegister = 0x02;
  static constexpr uint8_t kShuntVoltageRegister = 0x01;
  static constexpr float kShuntResistorOhms = 0.02f; //020R
};

#endif // SYSTEM_POWER_CONTROLLER_HPP
