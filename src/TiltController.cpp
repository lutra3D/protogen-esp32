#include "TiltController.hpp"

TiltController::TiltController(EmotionState &emotionState)
    : emotionState_(emotionState),
      tiltEnabled_(true),
      lastUpdateMillis_(0),
      tiltChangeMillis_(0),
      wasTilt_(false) {}

bool TiltController::begin() {
  Wire.begin(SDA, SCL, 400000);
  Wire.beginTransmission(0x68);
  if (Wire.endTransmission() == 0) {
    tiltEnabled_ = true;
    mpu6050_.reset(new MPU6050(Wire));
    mpu6050_->begin();
    return true;
  }
  tiltEnabled_ = false;
  Serial.println(F("MPU6050 not found. Tilt disabled"));
  return false;
}

void TiltController::update() {
  if (!tiltEnabled_) {
    return;
  }

  if (millis() - lastUpdateMillis_ < 100) {
    return;
  }

  mpu6050_->update();

  if (floatHelper_.isApproxEqual(mpu6050_->getAccX(), mpu6050_->getAccY(), mpu6050_->getAccZ(),
                                 tiltUpX_, tiltUpY_, tiltUpZ_, tiltTolerance_) && !wasTilt_) {
    Serial.println(F("[I] Tilt: UP!"));
    wasTilt_ = true;
    handleTiltChange(emotionState_.getTiltUpEmotion());
  } else if (floatHelper_.isApproxEqual(mpu6050_->getAccX(), mpu6050_->getAccY(), mpu6050_->getAccZ(),
                                        tiltSideX_, tiltSideY_, tiltSideZ_, tiltTolerance_) && !wasTilt_) {
    Serial.println(F("[I] Tilt: Side!"));
    wasTilt_ = true;
    handleTiltChange(emotionState_.getTiltSideEmotion());
  } else if ((wasTilt_ && (millis() - tiltChangeMillis_ > tiltAnimationMaxDuration_)) ||
             (wasTilt_ && floatHelper_.isApproxEqual(mpu6050_->getAccX(), mpu6050_->getAccY(), mpu6050_->getAccZ(),
                                                     tiltNeutralX_, tiltNeutralY_, tiltNeutralZ_, tiltTolerance_))) {
    Serial.println(F("[I] Tilt: Neutral!"));
    wasTilt_ = false;
  }

  lastUpdateMillis_ = millis();
}

bool TiltController::isEnabled() const {
  return tiltEnabled_;
}

String TiltController::readAcceleration() {
  if (!tiltEnabled_) {
    return F("Tilt is disabled");
  }
  mpu6050_->update();
  return String(mpu6050_->getAccX(), 2) + ";" + String(mpu6050_->getAccY(), 2) + ";" + String(mpu6050_->getAccZ(), 2);
}

void TiltController::handleTiltChange(const String &targetEmotion) {
  emotionState_.setCurrentEmotion(targetEmotion);
  tiltChangeMillis_ = millis();
}
