#include "Capabilities/CapabilityManager.hpp"

#include "Capabilities/BrightnessDownCapability.hpp"
#include "Capabilities/BrightnessUpCapability.hpp"



CapabilityManager::CapabilityManager(EarController &earController,
                                     std::function<void()> onSettingsChanged)
{
  capabilities_.push_back(std::unique_ptr<BrightnessUpCapability>(new BrightnessUpCapability(earController, onSettingsChanged)));
  capabilities_.push_back(std::unique_ptr<BrightnessDownCapability>(new BrightnessDownCapability(earController, onSettingsChanged)));
}

Capability *CapabilityManager::getCapabilityByName(const String &name) const
{
  for (const auto &capability : capabilities_)
  {
    if (capability->getName() == name)
    {
      return capability.get();
    }
  }

  return nullptr;
}

std::vector<String> CapabilityManager::getAvailableCapabilities() const
{
  std::vector<String> capabilityNames;
  capabilityNames.reserve(capabilities_.size());

  for (const auto &capability : capabilities_)
  {
    capabilityNames.push_back(capability->getName());
  }

  return capabilityNames;
}
