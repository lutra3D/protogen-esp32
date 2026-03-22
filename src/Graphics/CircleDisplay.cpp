#include "CircleDisplay.hpp"
#include <cmath>

CircleDisplay::CircleDisplay(int ledsPerCircle, float radius)
    : ledsPerCircle(ledsPerCircle), radius(radius) {}

RasterPoint CircleDisplay::getRasterPoint(int index, int length) const
{
  if (length <= 0 || (length % ledsPerCircle) != 0)
  {
    return {static_cast<float>(index), 0.0f};
  }

  const int ledIndex = index % ledsPerCircle;
  
  return getLocalCirclePoint(ledIndex);
}

RasterPoint CircleDisplay::getLocalCirclePoint(int ledIndex) const
{
    const float angle = (static_cast<float>(ledIndex) / static_cast<float>(ledsPerCircle)) * 2.0f * 3.14159265f;
    const float x = cosf(angle) * radius;
    const float y = sinf(angle) * radius;
    return {x, y};
}