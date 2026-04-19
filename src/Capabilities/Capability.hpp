#ifndef CAPABILITY_HPP
#define CAPABILITY_HPP

#include <Arduino.h>

class Capability
{
public:
  explicit Capability(String name);
  virtual ~Capability() = default;

  const String &getName() const;
  virtual bool handle() = 0;

private:
  String name_;
};

#endif // CAPABILITY_HPP
