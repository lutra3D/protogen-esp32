#include "Gradient.hpp"

Gradient::Gradient() : from(255, 255, 255), to(255, 255, 255), angle(0.0f), midpoint(0.5f) {}

Gradient::Gradient(const Color &from, const Color &to, float angle, float midpoint)
    : from(from),
      to(to),
      angle(angle),
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
    float angle,
    float midpoint)
{
  if (!from.setFromHex(fromHex) || !to.setFromHex(toHex))
  {
    return false;
  }
  this->angle = angle;
  this->midpoint = clampUnit(midpoint);

  return true;
}

Color Gradient::rasterize(int index, int length, CircleDisplay display) const
{
  if (length <= 1)
  {
    return from;
  }

  float normalizedDirectionX = cosf(angle);
  float normalizedDirectionY = sinf(angle);
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

  const RasterPoint currentPoint = display.getRasterPoint(index, length);

  float minProjection = 0.0f;
  float maxProjection = 0.0f;
  bool firstPoint = true;
  for (int ledIndex = 0; ledIndex < length; ++ledIndex)
  {
    const RasterPoint point = display.getRasterPoint(ledIndex, length);
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

  //Serial.print("Rasterizing LED "+String(red)+","+String(green)+","+String(blue)+" at position "+String(position)+" with blend "+String(blend)+", nx="+String(normalizedDirectionX)+", ny="+String(normalizedDirectionY)+"\n");

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
  gradientJson["angle"] = angle;
  gradientJson["midpoint"] = midpoint;
}

bool Gradient::deserialize(const JsonObject &obj, String &error)
{
  if (!obj["from"].is<String>() || !obj["to"].is<String>())
  {
    error = "Missing required 'from' or 'to' color fields.";
    return false;
  }

  String fromHex = obj["from"].as<String>();
  String toHex = obj["to"].as<String>();
  float angle = obj["angle"].is<float>() ? obj["angle"].as<float>() : 0.0f;
  float midpoint = obj["midpoint"].is<float>() ? obj["midpoint"].as<float>() : 0.5f;

  if (!setFromHex(fromHex, toHex, angle, midpoint))
  {
    error = "Invalid color format. Use #RRGGBB.";
    return false;
  }

  return true;
}
