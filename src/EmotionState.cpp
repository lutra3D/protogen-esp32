#include "EmotionState.hpp"

EmotionState::EmotionState()
    : currentEmotion_("/anims/neutral.gif"),
      previousEmotion_("/anims/neutral.gif"),
      tiltUpEmotion_("/anims/happy.gif"),
      tiltSideEmotion_("/anims/confused.gif") {}

const String EmotionState::getDisplayEmotion() const
{
  size_t lastSlash = currentEmotion_.lastIndexOf("/");
  String filename = (lastSlash == std::string::npos) ? currentEmotion_ : currentEmotion_.substring(lastSlash + 1);
  size_t lastDot = filename.lastIndexOf(".");

  if (lastDot == std::string::npos){
    return filename;
  }
  return filename.substring(0, lastDot);
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
