#ifndef FAN_SPEED_DOWN_CAPABILITY_HPP
#define FAN_SPEED_DOWN_CAPABILITY_HPP

#include <functional>

#include "Capabilities/Capability.hpp"
#include "FanController.hpp"

class FanSpeedDownCapability : public Capability
{
public:
  FanSpeedDownCapability(FanController &fanController,
                         std::function<void()> onSettingsChanged);

  bool handle() override;

private:
  FanController &fanController_;
  std::function<void()> onSettingsChanged_;
};

#endif // FAN_SPEED_DOWN_CAPABILITY_HPP
