#include "AnimationFile.hpp"

void AnimationFile::serialize(JsonVariant json) const
{
    if (json.isNull())
    {
        return;
    }

    json["name"] = name;
    json["path"] = path;
}

bool AnimationFile::deserialize(const JsonObject &object, String &error)
{
    if (!object["name"].is<String>())
    {
        error = F("AnimationFile 'name' is required and must be a string.");
        return false;
    }
    name = object["name"].as<String>();

    if (!object["path"].is<String>())
    {
        error = F("AnimationFile 'path' is required and must be a string.");
        return false;
    }
    path = object["path"].as<String>();

    return true;
}