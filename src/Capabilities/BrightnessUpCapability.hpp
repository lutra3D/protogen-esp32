#ifndef BRIGHTNESS_UP_CAPABILITY_HPP
#define BRIGHTNESS_UP_CAPABILITY_HPP

#include <functional>

#include "Capabilities/Capability.hpp"
#include "EarController.hpp"

class BrightnessUpCapability : public Capability
{
public:
  BrightnessUpCapability(EarController &earController,
                         std::function<void()> onSettingsChanged);

  bool handle() override;

private:
  EarController &earController_;
  std::function<void()> onSettingsChanged_;
};

#endif // BRIGHTNESS_UP_CAPABILITY_HPP
