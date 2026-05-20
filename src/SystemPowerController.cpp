#include "SystemPowerController.hpp"

SystemPowerController::SystemPowerController(uint8_t sdaPin, uint8_t sclPin)
    : sdaPin_(sdaPin),
      sclPin_(sclPin),
      enabled_(false) {}

bool SystemPowerController::begin() {
  Wire.begin(sdaPin_, sclPin_);
  Wire.beginTransmission(kIna226Address);
  enabled_ = (Wire.endTransmission() == 0);
  if (!enabled_) {
    Serial.println(F("INA226 not found. System power monitoring disabled"));
  }
  return enabled_;
}

bool SystemPowerController::isEnabled() const {
  return enabled_;
}

String SystemPowerController::readPowerInfo() {
  if (!enabled_) {
    return F("System power sensor is disabled");
  }

  uint16_t rawBusVoltage = 0;
  uint16_t rawShuntVoltage = 0;
  if (!readRegister16(kBusVoltageRegister, rawBusVoltage) || !readRegister16(kShuntVoltageRegister, rawShuntVoltage)) {
    return F("System power read error");
  }
  
  // INA219:
  // Bus voltage register: bits [15:3], LSB = 4 mV
  // Shunt voltage register: signed 16-bit, LSB = 10 uV
  // Shunt resistor: 0.02 ohm

  const float voltage = static_cast<float>(rawBusVoltage >> 3) * 0.004f;

  const int16_t signedShuntVoltage = static_cast<int16_t>(rawShuntVoltage);
  const float shuntVolts = static_cast<float>(signedShuntVoltage) * 0.00001f;

  const float currentAmps = shuntVolts / kShuntResistorOhms;
  const float currentMilliamps = currentAmps * 1000.0f;

  return String(voltage, 2) + F("V  ") + String(currentMilliamps, 0) + F("mA");
}

bool SystemPowerController::readRegister16(uint8_t reg, uint16_t &value) const {
  Wire.beginTransmission(kIna226Address);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) {
    return false;
  }

  if (Wire.requestFrom(static_cast<int>(kIna226Address), 2) != 2) {
    return false;
  }

  value = (static_cast<uint16_t>(Wire.read()) << 8) | static_cast<uint16_t>(Wire.read());
  return true;
}
