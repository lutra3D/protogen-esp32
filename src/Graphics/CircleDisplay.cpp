#include "CircleDisplay.hpp"

RasterPoint CircleDisplay::getRasterPoint(int index, int length) const
{
  if (length <= 0 || (length % kLedsPerCircle) != 0)
  {
    return {static_cast<float>(index), 0.0f};
  }

  const int ledIndex = index % kLedsPerCircle;
  
  return getLocalCirclePoint(ledIndex);
}

RasterPoint CircleDisplay::getLocalCirclePoint(int ledIndex) const
{
  switch (ledIndex)
  {
  case 0:
    return {-1.0f, 3.0f};
  case 1:
    return {0.0f, 3.0f};
  case 2:
    return {1.0f, 3.0f};
  case 3:
    return {2.0f, 2.0f};
  case 4:
    return {3.0f, 1.0f};
  case 5:
    return {3.0f, 0.0f};
  case 6:
    return {3.0f, -1.0f};
  case 7:
    return {2.0f, -2.0f};
  case 8:
    return {1.0f, -3.0f};
  case 9:
    return {0.0f, -3.0f};
  case 10:
    return {-1.0f, -3.0f};
  case 11:
    return {-2.0f, -2.0f};
  case 12:
    return {-3.0f, -1.0f};
  case 13:
    return {-3.0f, 0.0f};
  case 14:
    return {-3.0f, 1.0f};
  case 15:
  default:
    return {-3.0f, 2.0f};
  }
}