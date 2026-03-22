#ifndef GRADIENT_HPP
#define GRADIENT_HPP

#include <ArduinoJson.h>

#include "Color.hpp"
#include "CircleDisplay.hpp"

struct Gradient
{
  Color from;
  Color to;
  float directionX = 1.0f;
  float directionY = 0.0f;
  float midpoint = 0.5f;

public:
  explicit Gradient();
  Gradient(const Color &from, const Color &to, float directionX, float directionY, float midpoint);
  bool setFromHex(const String &fromHex, const String &toHex, float directionX, float directionY, float midpoint);
  Color rasterize(int index, int length, CircleDisplay display) const;
  void serialize(JsonVariant gradientJson) const;
  bool deserialize(const JsonObject &obj, String &error);

  private:
  float clampUnit(float value) const;
  uint8_t interpolateComponent(uint8_t from, uint8_t to, float factor) const;
};

enum class ColorMode
{
  Solid,
  Gradient,
};

#endif // GRADIENT_HPP