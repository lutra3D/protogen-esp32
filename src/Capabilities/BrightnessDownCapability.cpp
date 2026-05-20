#include "Capabilities/BrightnessDownCapability.hpp"

namespace
{
constexpr float kStepPercent = 20.0f;
constexpr float kMinPercent = 0.0f;
}

BrightnessDownCapability::BrightnessDownCapability(
    LedBrightnessController &brightnessController,
    std::function<void()> onSettingsChanged)
    : Capability(F("brightness_down")),
      brightnessController_(brightnessController),
      onSettingsChanged_(onSettingsChanged)
{
}

bool BrightnessDownCapability::handle()
{
  float nextBrightness = brightnessController_.getBrightnessPercent() - kStepPercent;
  if (nextBrightness < kMinPercent)
  {
    nextBrightness = kMinPercent;
  }

  brightnessController_.setBrightnessPercent(nextBrightness);

  if (onSettingsChanged_)
  {
    onSettingsChanged_();
  }

  return true;
}
