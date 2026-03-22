#include "Gradient.hpp"

namespace
{
constexpr int kLedsPerCircle = 16;
constexpr float kCircleSpacing = 4.0f;

struct RasterPoint
{
  float x;
  float y;
};

RasterPoint getLocalCirclePoint(int ledIndex)
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
    return {1.0f, 2.0f};
  case 4:
    return {1.0f, 1.0f};
  case 5:
    return {1.0f, 0.0f};
  case 6:
    return {1.0f, -1.0f};
  case 7:
    return {1.0f, -2.0f};
  case 8:
    return {1.0f, -3.0f};
  case 9:
    return {0.0f, -3.0f};
  case 10:
    return {-1.0f, -3.0f};
  case 11:
    return {-1.0f, -2.0f};
  case 12:
    return {-1.0f, -1.0f};
  case 13:
    return {-1.0f, 0.0f};
  case 14:
    return {-1.0f, 1.0f};
  case 15:
  default:
    return {-1.0f, 2.0f};
  }
}

RasterPoint getRasterPoint(int index, int length)
{
  if (length <= 0 || (length % kLedsPerCircle) != 0)
  {
    return {static_cast<float>(index), 0.0f};
  }

  const int circleCount = length / kLedsPerCircle;
  const int circleIndex = index / kLedsPerCircle;
  const int ledIndex = index % kLedsPerCircle;
  const float circleOffset = (static_cast<float>(circleIndex) - ((static_cast<float>(circleCount) - 1.0f) * 0.5f)) * kCircleSpacing;

  RasterPoint point = getLocalCirclePoint(ledIndex);
  point.x += circleOffset;
  return point;
}
} // namespace

Gradient::Gradient() : from(255, 255, 255), to(255, 255, 255), directionX(1.0f), directionY(0.0f), midpoint(0.5f) {}

Gradient::Gradient(const Color &from, const Color &to, float directionX, float directionY, float midpoint)
    : from(from),
      to(to),
      directionX(directionX),
      directionY(directionY),
      midpoint(midpoint) {}

float Gradient::clampUnit(float value) const
{
  if (value < 0.0f)
  {
    return 0.0f;
  }
  if (value > 1.0f)
  {
    return 1.0f;
  }
  return value;
}

uint8_t Gradient::interpolateComponent(uint8_t from, uint8_t to, float factor) const
{
  const float start = static_cast<float>(from);
  const float end = static_cast<float>(to);
  const float value = start + ((end - start) * clampUnit(factor));
  return static_cast<uint8_t>(roundf(value));
}

bool Gradient::setFromHex(
    const String &fromHex,
    const String &toHex,
    float directionX,
    float directionY,
    float midpoint)
{
  if (!from.setFromHex(fromHex) || !to.setFromHex(toHex))
  {
    return false;
  }
  this->directionX = directionX;
  this->directionY = directionY;
  this->midpoint = clampUnit(midpoint);

  return true;
}

Color Gradient::rasterizeColor(int index, int length) const
{
  if (length <= 1)
  {
    return from;
  }

  float normalizedDirectionX = directionX;
  float normalizedDirectionY = directionY;
  const float directionLength = sqrtf((normalizedDirectionX * normalizedDirectionX) + (normalizedDirectionY * normalizedDirectionY));
  if (directionLength <= 0.0f)
  {
    normalizedDirectionX = 1.0f;
    normalizedDirectionY = 0.0f;
  }
  else
  {
    normalizedDirectionX /= directionLength;
    normalizedDirectionY /= directionLength;
  }

  const RasterPoint currentPoint = getRasterPoint(index, length);

  float minProjection = 0.0f;
  float maxProjection = 0.0f;
  bool firstPoint = true;
  for (int ledIndex = 0; ledIndex < length; ++ledIndex)
  {
    const RasterPoint point = getRasterPoint(ledIndex, length);
    const float projection = (point.x * normalizedDirectionX) + (point.y * normalizedDirectionY);
    if (firstPoint)
    {
      minProjection = projection;
      maxProjection = projection;
      firstPoint = false;
      continue;
    }

    if (projection < minProjection)
    {
      minProjection = projection;
    }
    if (projection > maxProjection)
    {
      maxProjection = projection;
    }
  }

  const float currentProjection = (currentPoint.x * normalizedDirectionX) + (currentPoint.y * normalizedDirectionY);
  const float projectionRange = maxProjection - minProjection;
  float position = projectionRange > 0.0f
                       ? (currentProjection - minProjection) / projectionRange
                       : 0.0f;
  position = clampUnit(position);

  const float adjustedMidpoint = midpoint <= 0.0f
                                     ? 0.0001f
                                     : midpoint >= 1.0f
                                           ? 0.9999f
                                           : midpoint;

  float blend = 0.0f;
  if (position <= adjustedMidpoint)
  {
    blend = 0.5f * (position / adjustedMidpoint);
  }
  else
  {
    blend = 0.5f + (0.5f * ((position - adjustedMidpoint) / (1.0f - adjustedMidpoint)));
  }

  const uint8_t red = interpolateComponent(from.getRed(), to.getRed(), blend);
  const uint8_t green = interpolateComponent(from.getGreen(), to.getGreen(), blend);
  const uint8_t blue = interpolateComponent(from.getBlue(), to.getBlue(), blend);

  Color color(red, green, blue);
  return color;
}

void Gradient::serialize(JsonVariant gradientJson) const
{
  if (gradientJson.isNull())
  {
    return;
  }

  gradientJson["from"] = from.toHexString();
  gradientJson["to"] = to.toHexString();
  gradientJson["directionX"] = directionX;
  gradientJson["directionY"] = directionY;
  gradientJson["midpoint"] = midpoint;
}

bool Gradient::deserialize(const JsonObject &obj, String &error)
{
  if (!obj.containsKey("from") || !obj.containsKey("to"))
  {
    error = "Missing required 'from' or 'to' color fields.";
    return false;
  }

  String fromHex = obj["from"].as<String>();
  String toHex = obj["to"].as<String>();
  float directionX = obj.containsKey("directionX") ? obj["directionX"].as<float>() : 1.0f;
  float directionY = obj.containsKey("directionY") ? obj["directionY"].as<float>() : 0.0f;
  float midpoint = obj.containsKey("midpoint") ? obj["midpoint"].as<float>() : 0.5f;

  if (!setFromHex(fromHex, toHex, directionX, directionY, midpoint))
  {
    error = "Invalid color format. Use #RRGGBB.";
    return false;
  }

  return true;
}
