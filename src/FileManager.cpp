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

std::vector<AnimationFile> FileManager::getEmotions() const
{
  std::vector<AnimationFile> animations;

  File animationsDir = SPIFFS.open("/anims");
  if (!animationsDir || !animationsDir.isDirectory())
  {
    Serial.println(F("[E] Failed to open animation directory"));
    return animations;
  }

  File file = animationsDir.openNextFile();
  while (file)
  {
    AnimationFile info;
    info.path = file.path();
    info.name = FileHelper::GetNameOnly(info.path);

    if (info.path.endsWith(".gif"))
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
  const auto animations = getEmotions();
  for (const auto &anim : animations)
  {
    Serial.printf("Animation: %s, Path: %s\n", anim.name.c_str(), anim.path.c_str());
  }
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
