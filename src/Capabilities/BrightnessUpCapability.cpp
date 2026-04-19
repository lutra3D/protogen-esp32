#include "Capabilities/BrightnessUpCapability.hpp"

namespace
{
constexpr float kStepPercent = 20.0f;
constexpr float kMaxPercent = 100.0f;
}

BrightnessUpCapability::BrightnessUpCapability(
    EarController &earController,
    std::function<void()> onSettingsChanged)
    : Capability(F("brightness_up")),
      earController_(earController),
      onSettingsChanged_(onSettingsChanged)
{
}

bool BrightnessUpCapability::handle()
{
  float nextBrightness = earController_.getBrightnessPercent() + kStepPercent;
  if (nextBrightness > kMaxPercent)
  {
    nextBrightness = kMaxPercent;
  }

  earController_.setBrightnessPercent(nextBrightness);

  if (onSettingsChanged_)
  {
    onSettingsChanged_();
  }

  return true;
}
