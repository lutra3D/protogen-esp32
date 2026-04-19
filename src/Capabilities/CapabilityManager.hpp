#ifndef CAPABILITY_MANAGER_HPP
#define CAPABILITY_MANAGER_HPP

#include <functional>
#include <memory>
#include <vector>

#include "Capabilities/Capability.hpp"
#include "EarController.hpp"

class CapabilityManager
{
public:
  CapabilityManager(EarController &earController,
                    std::function<void()> onSettingsChanged);

  Capability *getCapabilityByName(const String &name) const;
  std::vector<String> getAvailableCapabilities() const;

private:
  std::vector<std::unique_ptr<Capability>> capabilities_;
};

#endif // CAPABILITY_MANAGER_HPP
