#ifndef FILE_MANAGER_HPP
#define FILE_MANAGER_HPP

#include <SPIFFS.h>

#include <Arduino.h>
#include <vector>

#include "Model/AnimationFile.hpp"

class FileManager {
public:
  bool begin(bool formatOnFail = true);
  std::vector<Model::File> getAnimationFiles() const;
  std::vector<Model::File> getFiles(const String &filter = "") const;
  void printEmotions() const;

  bool exists(const String &path) const;
  bool readFile(const String &path, String &content) const;
  bool writeFile(const String &path, const uint8_t *data, size_t len,
                 bool append = false) const;
  bool removeFile(const String &path) const;
  bool renameFile(const String &from, const String &to) const;
};

#endif // FILE_MANAGER_HPP
