#ifndef BRIGHTNESS_DOWN_CAPABILITY_HPP
#define BRIGHTNESS_DOWN_CAPABILITY_HPP

#include <functional>

#include "Capabilities/Capability.hpp"
#include "EarController.hpp"

class BrightnessDownCapability : public Capability
{
public:
  BrightnessDownCapability(EarController &earController,
                           std::function<void()> onSettingsChanged);

  bool handle() override;

private:
  EarController &earController_;
  std::function<void()> onSettingsChanged_;
};

#endif // BRIGHTNESS_DOWN_CAPABILITY_HPP
