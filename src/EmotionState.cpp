#include "EmotionState.hpp"

EmotionState::EmotionState()
    : currentEmotion_("/anims/neutral.gif"),
      previousEmotion_("/anims/neutral.gif"),
      tiltUpEmotion_("/anims/happy.gif"),
      tiltSideEmotion_("/anims/confused.gif")
{
  seedEmotionDefinitions({
      EmotionDefinition("Blush", "/anims/blush.gif", Color(0, 0, 0), Gradient(Color(0, 0, 0), Color(255, 192, 203), 0.0f, 0.5f), ColorMode::Gradient),
      EmotionDefinition("Cute", "/anims/cute.gif", Color(255, 192, 203), Gradient(), ColorMode::Solid),
      EmotionDefinition("Evil", "/anims/evil.gif", Color(0, 0, 0), Gradient(), ColorMode::Solid),
      EmotionDefinition("Happy", "/anims/happy.gif", Color(255, 255, 0), Gradient(), ColorMode::Solid),
      EmotionDefinition("Neutral", "/anims/neutral.gif", Color(255, 255, 255), Gradient(), ColorMode::Solid),
      EmotionDefinition("Sad", "/anims/sad.gif", Color(0, 0, 255), Gradient(), ColorMode::Solid),
  });
}

const String EmotionState::getDisplayEmotion() const
{
  const EmotionDefinition *emotion = getCurrentEmotionDefinition();
  if (emotion != nullptr && emotion->name.length())
  {
    return emotion->name;
  }
  return FileHelper::GetNameOnly(currentEmotion_);
}

const String &EmotionState::getCurrentEmotion() const
{
  return currentEmotion_;
}

const String &EmotionState::getPreviousEmotion() const
{
  return previousEmotion_;
}

void EmotionState::setCurrentEmotion(const String &emotionName)
{
  previousEmotion_ = currentEmotion_;

  const EmotionDefinition *byName = getEmotionDefinitionByName(emotionName);
  if (byName != nullptr)
  {
    currentEmotion_ = byName->path;
    return;
  }

  const EmotionDefinition *byPath = getEmotionDefinitionByPath(emotionName);
  if (byPath != nullptr)
  {
    currentEmotion_ = byPath->path;
    return;
  }

  currentEmotion_ = emotionName;
}

const String &EmotionState::getTiltUpEmotion() const
{
  return tiltUpEmotion_;
}

const String &EmotionState::getTiltSideEmotion() const
{
  return tiltSideEmotion_;
}

void EmotionState::setTiltUpEmotion(const String &emotionName)
{
  tiltUpEmotion_ = emotionName;
}

void EmotionState::setTiltSideEmotion(const String &emotionName)
{
  tiltSideEmotion_ = emotionName;
}

const std::vector<EmotionDefinition> &EmotionState::getEmotionDefinitions() const
{
  return emotionDefinitions_;
}

const EmotionDefinition *EmotionState::getEmotionDefinitionByName(const String &name) const
{
  const int index = findEmotionIndexByName(name);
  if (index < 0)
  {
    return nullptr;
  }
  return &emotionDefinitions_[static_cast<size_t>(index)];
}

const EmotionDefinition *EmotionState::getEmotionDefinitionByPath(const String &path) const
{
  const int index = findEmotionIndexByPath(path);
  if (index < 0)
  {
    return nullptr;
  }
  return &emotionDefinitions_[static_cast<size_t>(index)];
}

const EmotionDefinition *EmotionState::getCurrentEmotionDefinition() const
{
  return getEmotionDefinitionByPath(currentEmotion_);
}

bool EmotionState::upsertEmotionDefinition(const EmotionDefinition &emotion, bool overwriteExisting)
{
  const int nameIndex = findEmotionIndexByName(emotion.name);
  if (nameIndex >= 0)
  {
    if (!overwriteExisting)
    {
      return false;
    }
    emotionDefinitions_[static_cast<size_t>(nameIndex)] = emotion;
    return true;
  }

  const int pathIndex = findEmotionIndexByPath(emotion.path);
  if (pathIndex >= 0)
  {
    if (!overwriteExisting)
    {
      return false;
    }
    emotionDefinitions_[static_cast<size_t>(pathIndex)] = emotion;
    return true;
  }

  emotionDefinitions_.push_back(emotion);
  return true;
}

bool EmotionState::removeEmotionDefinitionByName(const String &name)
{
  const int index = findEmotionIndexByName(name);
  if (index < 0)
  {
    return false;
  }

  const String removedPath = emotionDefinitions_[static_cast<size_t>(index)].path;
  emotionDefinitions_.erase(emotionDefinitions_.begin() + index);

  if (currentEmotion_ == removedPath)
  {
    currentEmotion_ = previousEmotion_;
  }

  return true;
}

void EmotionState::seedEmotionDefinitions(const std::vector<EmotionDefinition> &emotions)
{
  emotionDefinitions_.clear();
  for (const auto &emotion : emotions)
  {
    emotionDefinitions_.push_back(emotion);
  }
}

int EmotionState::findEmotionIndexByName(const String &name) const
{
  for (size_t index = 0; index < emotionDefinitions_.size(); ++index)
  {
    if (emotionDefinitions_[index].name == name)
    {
      return static_cast<int>(index);
    }
  }
  return -1;
}

int EmotionState::findEmotionIndexByPath(const String &path) const
{
  for (size_t index = 0; index < emotionDefinitions_.size(); ++index)
  {
    if (emotionDefinitions_[index].path == path)
    {
      return static_cast<int>(index);
    }
  }
  return -1;
}
