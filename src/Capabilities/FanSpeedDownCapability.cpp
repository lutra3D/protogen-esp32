#include "Capabilities/FanSpeedDownCapability.hpp"

namespace
{
constexpr float kStepPercent = 20.0f;
constexpr float kMinPercent = 0.0f;
}

FanSpeedDownCapability::FanSpeedDownCapability(
    FanController &fanController,
    std::function<void()> onSettingsChanged)
    : Capability(F("fan_speed_down")),
      fanController_(fanController),
      onSettingsChanged_(onSettingsChanged)
{
}

bool FanSpeedDownCapability::handle()
{
  float nextDutyCycle = fanController_.getDutyCyclePercent() - kStepPercent;
  if (nextDutyCycle < kMinPercent)
  {
    nextDutyCycle = kMinPercent;
  }

  fanController_.setDutyCyclePercent(nextDutyCycle);

  if (onSettingsChanged_)
  {
    onSettingsChanged_();
  }

  return true;
}
