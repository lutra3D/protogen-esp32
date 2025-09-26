#ifndef STATUS_DISPLAY_HPP
#define STATUS_DISPLAY_HPP

#ifndef SH1106_128_64
#define SH1106_128_64
#endif

#include <Adafruit_SH1106.h>
#include <Arduino.h>

class EmotionState;
class FanController;
class EarController;

class StatusDisplay {
public:
  StatusDisplay(EmotionState &emotionState,
                FanController &fanController,
                EarController &earController,
                int8_t resetPin = -1,
                uint8_t i2cAddress = 0x3C);

  bool begin();
  void update();
  void setUpdateInterval(uint32_t intervalMs);

private:
  void drawStatus(bool force = false);
  String formatFanSpeedPercent(int dutyCycle) const;
  String formatEarColor(uint8_t red, uint8_t green, uint8_t blue) const;
  String getAccessPointIp() const;

  EmotionState &emotionState_;
  FanController &fanController_;
  EarController &earController_;
  Adafruit_SH1106 display_;
  uint8_t i2cAddress_;
  uint32_t lastUpdateMs_;
  uint32_t updateIntervalMs_;

  String lastEmotion_;
  int lastDutyCycle_;
  uint8_t lastRed_;
  uint8_t lastGreen_;
  uint8_t lastBlue_;
  uint8_t lastBrightness_;
  String lastApIp_;
};

#endif // STATUS_DISPLAY_HPP
