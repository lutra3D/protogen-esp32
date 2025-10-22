#ifndef DISPLAY_MANAGER_HPP
#define DISPLAY_MANAGER_HPP

#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include <Arduino.h>

#include "EarController.hpp"
#include "EmotionState.hpp"
#include "FanController.hpp"

class DisplayManager {
public:
  DisplayManager(uint8_t sdaPin, uint8_t sclPin, EmotionState &emotionState,
                 FanController &fanController, EarController &earController);

  void begin();
  void update();

private:
  void renderStatus();
  String formatEarInfo() const;

  uint8_t sdaPin_;
  uint8_t sclPin_;
  EmotionState &emotionState_;
  FanController &fanController_;
  EarController &earController_;
  Adafruit_SH1106 display_;
};

#endif // DISPLAY_MANAGER_HPP
