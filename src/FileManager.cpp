#include "FileManager.hpp"

#include "float_helper.hpp"

namespace {

void collectFilesRecursive(File directory, const String &normalizedFilter,
                           std::vector<Model::File> &files)
{
  if (!directory || !directory.isDirectory())
  {
    return;
  }

  File entry = directory.openNextFile();
  while (entry)
  {
    if (entry.isDirectory())
    {
      collectFilesRecursive(entry, normalizedFilter, files);
      entry.close();
      entry = directory.openNextFile();
      continue;
    }

    Model::File info;
    info.path = entry.path();
    info.name = FileHelper::GetNameOnly(info.path);

    if (normalizedFilter.isEmpty() || info.name.indexOf(normalizedFilter) >= 0 ||
        info.path.indexOf(normalizedFilter) >= 0)
    {
      files.push_back(std::move(info));
    }

    entry.close();
    entry = directory.openNextFile();
  }
}

} // namespace

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
  std::vector<Model::File> files;

  File rootDir = SPIFFS.open("/");
  if (!rootDir || !rootDir.isDirectory())
  {
    Serial.println(F("[E] Failed to open SPIFFS root directory"));
    return files;
  }

  const String normalizedFilter = String(filter);

  collectFilesRecursive(rootDir, normalizedFilter, files);

  rootDir.close();
  return files;
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
