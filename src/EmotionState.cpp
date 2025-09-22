#include "EmotionState.hpp"

EmotionState::EmotionState()
    : currentEmotion_("/neutral.gif"),
      previousEmotion_("/neutral.gif"),
      tiltUpEmotion_("/happy.gif"),
      tiltSideEmotion_("/confused.gif") {}

const String &EmotionState::getCurrentEmotion() const {
  return currentEmotion_;
}

const String &EmotionState::getPreviousEmotion() const {
  return previousEmotion_;
}

void EmotionState::setCurrentEmotion(const String &emotionName) {
  previousEmotion_ = currentEmotion_;
  currentEmotion_ = emotionName;
}

const String &EmotionState::getTiltUpEmotion() const {
  return tiltUpEmotion_;
}

const String &EmotionState::getTiltSideEmotion() const {
  return tiltSideEmotion_;
}

void EmotionState::setTiltUpEmotion(const String &emotionName) {
  tiltUpEmotion_ = emotionName;
}

void EmotionState::setTiltSideEmotion(const String &emotionName) {
  tiltSideEmotion_ = emotionName;
}
