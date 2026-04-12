#ifndef ANIMATION_FILE_HPP
#define ANIMATION_FILE_HPP

#include <Arduino.h>
#include <ArduinoJson.h>

namespace Model {

struct File {
  String name;
  String path;

  void serialize(JsonVariant json) const;
  bool deserialize(const JsonObject &object, String &error);
};

} // namespace Model

#endif // ANIMATION_FILE_HPP
