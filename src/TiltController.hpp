#ifndef TILT_CONTROLLER_HPP
#define TILT_CONTROLLER_HPP

#include <Arduino.h>
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <memory>

#include "EmotionState.hpp"
#include "float_helper.hpp"

class TiltController {
public:
  explicit TiltController(EmotionState &emotionState, uint8_t sdaPin, uint8_t sclPin);

  bool begin();
  void update();

  bool isEnabled() const;
  String readAcceleration();

private:
  void handleTiltChange(const String &targetEmotion);

  EmotionState &emotionState_;
  uint8_t sdaPin_;
  uint8_t sclPin_;
  std::unique_ptr<MPU6050> mpu6050_;
  bool tiltEnabled_;
  unsigned long lastUpdateMillis_;
  unsigned long tiltChangeMillis_;
  bool wasTilt_;
  FloatHelper floatHelper_;

  const float tiltNeutralX_ = 0.3f;
  const float tiltNeutralY_ = 0.92f;
  const float tiltNeutralZ_ = 0.33f;
  const float tiltSideX_ = 0.05f;
  const float tiltSideY_ = 0.7f;
  const float tiltSideZ_ = 0.73f;
  const float tiltUpX_ = -0.17f;
  const float tiltUpY_ = 0.99f;
  const float tiltUpZ_ = 0.11f;
  const float tiltTolerance_ = 0.1f;
  const unsigned long tiltAnimationMaxDuration_ = 8000UL;
};

#endif // TILT_CONTROLLER_HPP
