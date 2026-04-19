#include "Capabilities/Capability.hpp"

#include <utility>

Capability::Capability(String name)
    : name_(std::move(name))
{
}

const String &Capability::getName() const
{
  return name_;
}
