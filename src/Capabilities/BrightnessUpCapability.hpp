#ifndef BRIGHTNESS_UP_CAPABILITY_HPP
#define BRIGHTNESS_UP_CAPABILITY_HPP

#include <functional>

#include "Capabilities/Capability.hpp"
#include "LedBrightnessController.hpp"

class BrightnessUpCapability : public Capability
{
public:
  BrightnessUpCapability(LedBrightnessController &brightnessController,
                         std::function<void()> onSettingsChanged);

  bool handle() override;

private:
  LedBrightnessController &brightnessController_;
  std::function<void()> onSettingsChanged_;
};

#endif // BRIGHTNESS_UP_CAPABILITY_HPP
