#include "Ear.hpp"

Ear::Ear() : color_(0,0,0), gradient_(), colorMode_(ColorMode::Solid) {}

void Ear::setColor(uint8_t red, uint8_t green, uint8_t blue) {
  color_.set(red, green, blue);
  colorMode_ = ColorMode::Solid;
}

void Ear::setColor(const Color &color)
{
  color_ = color;
  colorMode_ = ColorMode::Solid;
}

bool Ear::setColorFromHex(const String &hex) {
  if (!color_.setFromHex(hex)) {
    return false;
  }
  colorMode_ = ColorMode::Solid;
  return true;
}

const Color &Ear::getColor() const {
  return color_;
}

String Ear::getColorHexString() const {
  return color_.toHexString();
}

bool Ear::setGradientFromHex(const String &fromHex, const String &toHex, float angle, float midpoint)
{
    if (!gradient_.setFromHex(fromHex, toHex, angle, midpoint)) {
        return false;
    }
    colorMode_ = ColorMode::Solid;
    return true;
}

void Ear::setGradient(const Gradient &gradient)
{
  gradient_ = gradient;
  colorMode_ = ColorMode::Gradient;
}

const Gradient &Ear::getGradient() const
{
  return gradient_;
}

ColorMode Ear::getColorMode() const
{
  return colorMode_;
}


void Ear::serialize(JsonVariant json) const {
  if (json.isNull()) {
    return;
  }

  json["mode"] = colorMode_ == ColorMode::Gradient ? "gradient" : "solid";
  json["color"] = getColorHexString();

  JsonObject gradientJson = json["gradient"].to<JsonObject>();
  gradient_.serialize(gradientJson);
}

bool Ear::deserialize(const JsonObject &object, String &error)
{
  if (object["mode"].is<String>())
  {
    const String mode = object["mode"].as<String>();
    colorMode_ = mode == "gradient" ? ColorMode::Gradient : ColorMode::Solid;
  }

  if (object["color"].is<String>())
  {
    const String colorHex = object["color"].as<String>();
    if (!color_.setFromHex(colorHex))
    {
      error = F("Invalid 'color' hex string.");
      return false;
    }
  }


  if (object["gradient"].is<JsonObject>())
  {
    Gradient gradient;
    if (!gradient.deserialize(object["gradient"].as<JsonObject>(), error))
    {
      return false;
    }
    gradient_ = gradient;
  }

  if (colorMode_ == ColorMode::Gradient)
  {
    setGradient(gradient_);
  }
  else
  {
    setColor(color_);
  }

  return true;
}
