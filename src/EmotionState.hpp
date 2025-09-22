#ifndef EMOTION_STATE_HPP
#define EMOTION_STATE_HPP

#include <Arduino.h>

class EmotionState {
public:
  EmotionState();

  const String &getCurrentEmotion() const;
  const String &getPreviousEmotion() const;

  void setCurrentEmotion(const String &emotionName);

  const String &getTiltUpEmotion() const;
  const String &getTiltSideEmotion() const;

  void setTiltUpEmotion(const String &emotionName);
  void setTiltSideEmotion(const String &emotionName);

private:
  String currentEmotion_;
  String previousEmotion_;
  String tiltUpEmotion_;
  String tiltSideEmotion_;
};

#endif // EMOTION_STATE_HPP
