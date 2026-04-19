#include "Capabilities/FanSpeedUpCapability.hpp"

namespace
{
constexpr float kStepPercent = 20.0f;
constexpr float kMaxPercent = 100.0f;
}

FanSpeedUpCapability::FanSpeedUpCapability(
    FanController &fanController,
    std::function<void()> onSettingsChanged)
    : Capability(F("fan_speed_up")),
      fanController_(fanController),
      onSettingsChanged_(onSettingsChanged)
{
}

bool FanSpeedUpCapability::handle()
{
  float nextDutyCycle = fanController_.getDutyCyclePercent() + kStepPercent;
  if (nextDutyCycle > kMaxPercent)
  {
    nextDutyCycle = kMaxPercent;
  }

  fanController_.setDutyCyclePercent(nextDutyCycle);

  if (onSettingsChanged_)
  {
    onSettingsChanged_();
  }

  return true;
}
