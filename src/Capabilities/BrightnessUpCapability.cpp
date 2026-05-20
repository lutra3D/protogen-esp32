#include "Capabilities/BrightnessUpCapability.hpp"

namespace
{
constexpr float kStepPercent = 20.0f;
constexpr float kMaxPercent = 100.0f;
}

BrightnessUpCapability::BrightnessUpCapability(
    LedBrightnessController &brightnessController,
    std::function<void()> onSettingsChanged)
    : Capability(F("brightness_up")),
      brightnessController_(brightnessController),
      onSettingsChanged_(onSettingsChanged)
{
}

bool BrightnessUpCapability::handle()
{
  float nextBrightness = brightnessController_.getBrightnessPercent() + kStepPercent;
  if (nextBrightness > kMaxPercent)
  {
    nextBrightness = kMaxPercent;
  }

  brightnessController_.setBrightnessPercent(nextBrightness);

  if (onSettingsChanged_)
  {
    onSettingsChanged_();
  }

  return true;
}
