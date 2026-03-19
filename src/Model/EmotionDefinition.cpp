#include "EmotionDefinition.hpp"

EmotionDefinition::EmotionDefinition()
    : name(""),
      path(""),
      earColor(Color(0, 0, 0)),
      earGradient(Gradient()),
      earColorMode(ColorMode::Solid)
{
}

EmotionDefinition::EmotionDefinition(const String &name, const String &path, const Color &earColor, const Gradient &earGradient, ColorMode earColorMode)
    : name(name),
      path(path),
      earColor(earColor),
      earGradient(earGradient),
      earColorMode(earColorMode)
{
}

void EmotionDefinition::serialize(JsonVariant object) const
{
    if (object.isNull())
    {
        return;
    }

    object["name"] = name;
    object["path"] = path;

    auto gradient = object["gradient"].to<JsonObject>();
    earGradient.serialize(gradient);

    object["earColorMode"] = earColorMode == ColorMode::Gradient
                                 ? "gradient"
                                 : "solid";

    object["earColor"] = earColor.toHexString();
}

bool EmotionDefinition::deserialize(const JsonObject &object, String &error)
{
    if (!object.containsKey("name") || !object["name"].is<String>())
    {
        error = F("Emotion 'name' is required and must be a string.");
        return false;
    }
    name = object["name"].as<String>();

    if (!object.containsKey("path") || !object["path"].is<String>())
    {
        error = F("Emotion 'path' is required and must be a string.");
        return false;
    }
    path = object["path"].as<String>();

    if (object.containsKey("earColorMode"))
    {
        String modeStr = object["earColorMode"].as<String>();
        earColorMode = modeStr == "gradient"
                           ? ColorMode::Gradient
                           : ColorMode::Solid;
    }

    if (object.containsKey("earColor"))
    {
        String colorHex = object["earColor"].as<String>();
        if (!earColor.setFromHex(colorHex))
        {
            error = F("Invalid 'earColor' hex string.");
            return false;
        }
    }

    if (object.containsKey("gradient") && object["gradient"].is<JsonObject>())
    {
        JsonObject gradientObj = object["gradient"].as<JsonObject>();
        if (!earGradient.deserialize(gradientObj, error))
        {
            return false;
        }
    }

    return true;
}