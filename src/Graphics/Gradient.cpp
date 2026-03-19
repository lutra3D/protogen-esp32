#include "Gradient.hpp"

Gradient::Gradient() : from(255, 255, 255), to(255, 255, 255), directionX(1.0f), directionY(0.0f), midpoint(0.5f) {}

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
  directionX = directionX;
  directionY = directionY;
  midpoint = clampUnit(midpoint);

  return true;
}

Color Gradient::rasterizeColor(int index, int length) const
{
    float position = length > 1
                         ? static_cast<float>(index) / static_cast<float>(length - 1)
                         : 0.0f;

      const float directionInfluence = (this->directionX + this->directionY) * 0.5f;
      position = clampUnit(position + (directionInfluence * 0.25f));

      float blend = 0.0f;
      if (this->midpoint <= 0.0f)
      {
        blend = 1.0f;
      }
      else if (this->midpoint >= 1.0f)
      {
        blend = 0.0f;
      }
      else
      {
        blend = position / this->midpoint;
      }
      blend = clampUnit(blend);

      const uint8_t red = interpolateComponent(from.getRed(), to.getRed(), blend);
      const uint8_t green = interpolateComponent(from.getGreen(), to.getGreen(), blend);
      const uint8_t blue = interpolateComponent(from.getBlue(), to.getBlue(), blend);

      Color color(red, green, blue);
      return color;
}

void Gradient::serialize(JsonVariant gradientJson) const {
  if (gradientJson.isNull()) {
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
  if (!obj.containsKey("from") || !obj.containsKey("to")) {
    error = "Missing required 'from' or 'to' color fields.";
    return false;
  }

  String fromHex = obj["from"].as<String>();
  String toHex = obj["to"].as<String>();
  float directionX = obj.containsKey("directionX") ? obj["directionX"].as<float>() : 1.0f;
  float directionY = obj.containsKey("directionY") ? obj["directionY"].as<float>() : 0.0f;
  float midpoint = obj.containsKey("midpoint") ? obj["midpoint"].as<float>() : 0.5f;

  if (!setFromHex(fromHex, toHex, directionX, directionY, midpoint)) {
    error = "Invalid color format. Use #RRGGBB.";
    return false;
  }

  return true;
}
