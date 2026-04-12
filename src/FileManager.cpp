#include "FileManager.hpp"

#include "float_helper.hpp"

bool FileManager::begin(bool formatOnFail)
{
  if (!SPIFFS.begin(formatOnFail))
  {
    Serial.println(F("SPIFFS mount failed!"));
    return false;
  }

  return true;
}

std::vector<Model::File> FileManager::getAnimationFiles() const
{
  return getFiles();
}

std::vector<Model::File> FileManager::getFiles(const String &filter) const
{
  std::vector<Model::File> animations;

  File animationsDir = SPIFFS.open("/anims");
  if (!animationsDir || !animationsDir.isDirectory())
  {
    Serial.println(F("[E] Failed to open animation directory"));
    return animations;
  }

  const String normalizedFilter = String(filter);
  File file = animationsDir.openNextFile();
  while (file)
  {
    Model::File info;
    info.path = file.path();
    info.name = FileHelper::GetNameOnly(info.path);

    if (info.path.endsWith(".gif") &&
        (normalizedFilter.isEmpty() || info.name.indexOf(normalizedFilter) >= 0 ||
         info.path.indexOf(normalizedFilter) >= 0))
    {
      animations.push_back(std::move(info));
    }

    file.close();
    file = animationsDir.openNextFile();
  }

  animationsDir.close();
  return animations;
}

void FileManager::printEmotions() const
{
  const auto animations = getAnimationFiles();
  for (const auto &anim : animations)
  {
    Serial.printf("Animation: %s, Path: %s\n", anim.name.c_str(), anim.path.c_str());
  }
}

bool FileManager::exists(const String &path) const
{
  return SPIFFS.exists(path);
}

bool FileManager::readFile(const String &path, String &content) const
{
  File file = SPIFFS.open(path, FILE_READ);
  if (!file)
  {
    return false;
  }

  content = file.readString();
  file.close();
  return true;
}

bool FileManager::writeFile(const String &path, const uint8_t *data, size_t len,
                            bool append) const
{
  const char *mode = append ? FILE_APPEND : FILE_WRITE;
  File file = SPIFFS.open(path, mode);
  if (!file)
  {
    return false;
  }

  const size_t written = file.write(data, len);
  file.close();
  return written == len;
}

bool FileManager::removeFile(const String &path) const
{
  return SPIFFS.remove(path);
}

bool FileManager::renameFile(const String &from, const String &to) const
{
  return SPIFFS.rename(from, to);
}
