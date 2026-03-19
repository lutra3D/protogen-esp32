#ifndef EMOTIONDEFINITION_HPP
#define EMOTIONDEFINITION_HPP

#include <Arduino.h>

#include "../Graphics/Brightness.hpp"
#include "../Graphics/Color.hpp"
#include "../Graphics/Gradient.hpp"
  
struct EmotionDefinition {
    String name;
    String path;
    Color earColor;
    Gradient earGradient;
    ColorMode earColorMode;

    EmotionDefinition();
    EmotionDefinition(const String &name, const String &path, const Color &earColor, const Gradient &earGradient, ColorMode earColorMode);
    void serialize(JsonVariant json) const;
    bool deserialize(const JsonObject &object, String &error);
};

#endif //EMOTIONDEFINITION_HPP