#ifndef BRIGHTNESS_DOWN_CAPABILITY_HPP
#define BRIGHTNESS_DOWN_CAPABILITY_HPP

#include <functional>

#include "Capabilities/Capability.hpp"
#include "LedBrightnessController.hpp"

class BrightnessDownCapability : public Capability
{
public:
  BrightnessDownCapability(LedBrightnessController &brightnessController,
                           std::function<void()> onSettingsChanged);

  bool handle() override;

private:
  LedBrightnessController &brightnessController_;
  std::function<void()> onSettingsChanged_;
};

#endif // BRIGHTNESS_DOWN_CAPABILITY_HPP
