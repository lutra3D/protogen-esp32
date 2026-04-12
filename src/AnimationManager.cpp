#include "AnimationManager.hpp"

AnimationManager::AnimationManager(int panelResX, int panelResY, int panelChainLength)
    : fileManager_(),
      faceDisplay_(panelResX, panelResY, panelChainLength)
{
}

bool AnimationManager::begin()
{
  if (!fileManager_.begin())
  {
    return false;
  }

  if (!faceDisplay_.begin())
  {
    return false;
  }

  fileManager_.printEmotions();
  return true;
}

void AnimationManager::playEmotion(const String &emotionPath)
{
  faceDisplay_.playEmotion(emotionPath);
}

std::vector<AnimationFile> AnimationManager::getEmotions() const
{
  return fileManager_.getEmotions();
}

void AnimationManager::printEmotions() const
{
  fileManager_.printEmotions();
}
