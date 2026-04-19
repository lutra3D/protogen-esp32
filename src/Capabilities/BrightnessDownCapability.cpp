#include "Capabilities/BrightnessDownCapability.hpp"

namespace
{
constexpr float kStepPercent = 20.0f;
constexpr float kMinPercent = 0.0f;
}

BrightnessDownCapability::BrightnessDownCapability(
    EarController &earController,
    std::function<void()> onSettingsChanged)
    : Capability(F("brightness_down")),
      earController_(earController),
      onSettingsChanged_(onSettingsChanged)
{
}

bool BrightnessDownCapability::handle()
{
  float nextBrightness = earController_.getBrightnessPercent() - kStepPercent;
  if (nextBrightness < kMinPercent)
  {
    nextBrightness = kMinPercent;
  }

  earController_.setBrightnessPercent(nextBrightness);

  if (onSettingsChanged_)
  {
    onSettingsChanged_();
  }

  return true;
}
