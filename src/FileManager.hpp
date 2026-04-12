#ifndef FILE_MANAGER_HPP
#define FILE_MANAGER_HPP

#include <SPIFFS.h>

#include <Arduino.h>
#include <vector>

#include "Model/AnimationFile.hpp"

class FileManager {
public:
  bool begin(bool formatOnFail = true);
  std::vector<AnimationFile> getEmotions() const;
  void printEmotions() const;
  bool readFile(const String &path, String &content) const;
};

#endif // FILE_MANAGER_HPP
