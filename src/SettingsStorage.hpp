#ifndef SETTINGS_STORAGE_HPP
#define SETTINGS_STORAGE_HPP

#include <Arduino.h>

#include "LedBrightnessController.hpp"
#include "EmotionState.hpp"
#include "FanController.hpp"

class SettingsStorage
{
public:
  SettingsStorage(EmotionState &emotionState, FanController &fanController,
                  LedBrightnessController &brightnessController);

  bool load();
  bool save() const;

private:
  static constexpr const char *kSettingsPath = "/settings.json";

  EmotionState &emotionState_;
  FanController &fanController_;
  LedBrightnessController &brightnessController_;
};

#endif // SETTINGS_STORAGE_HPP
