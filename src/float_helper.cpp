#include "float_helper.hpp"

bool FloatHelper::isApproxEqual(const float ax, const float ay, const float az, const float bx, const float by, const float bz, const float tiltTol) const {
  return (fabs(ax-bx) < tiltTol && fabs(ay-by) < tiltTol && fabs(az-bz) < tiltTol);
}

float FloatHelper::mapfloat(float x, float in_min, float in_max, float out_min, float out_max) const {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}