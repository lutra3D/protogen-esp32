#ifndef FAN_SPEED_UP_CAPABILITY_HPP
#define FAN_SPEED_UP_CAPABILITY_HPP

#include <functional>

#include "Capabilities/Capability.hpp"
#include "FanController.hpp"

class FanSpeedUpCapability : public Capability
{
public:
  FanSpeedUpCapability(FanController &fanController,
                       std::function<void()> onSettingsChanged);

  bool handle() override;

private:
  FanController &fanController_;
  std::function<void()> onSettingsChanged_;
};

#endif // FAN_SPEED_UP_CAPABILITY_HPP
