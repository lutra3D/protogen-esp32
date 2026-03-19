#ifndef EMOTION_STATE_HPP
#define EMOTION_STATE_HPP

#include <Arduino.h>

#include <vector>

#include "float_helper.hpp"
#include "Model/EmotionDefinition.hpp"

class EmotionState {
public:
  EmotionState();

  const String &getCurrentEmotion() const;
  const String &getPreviousEmotion() const;
  const String getDisplayEmotion() const;

  void setCurrentEmotion(const String &emotionName);

  const String &getTiltUpEmotion() const;
  const String &getTiltSideEmotion() const;

  void setTiltUpEmotion(const String &emotionName);
  void setTiltSideEmotion(const String &emotionName);

  const std::vector<EmotionDefinition> &getEmotionDefinitions() const;
  const EmotionDefinition *getEmotionDefinitionByName(const String &name) const;
  const EmotionDefinition *getEmotionDefinitionByPath(const String &path) const;
  const EmotionDefinition *getCurrentEmotionDefinition() const;
  bool upsertEmotionDefinition(const EmotionDefinition &emotion, bool overwriteExisting);
  bool removeEmotionDefinitionByName(const String &name);
  void seedEmotionDefinitions(const std::vector<EmotionDefinition> &emotions);

private:
  int findEmotionIndexByName(const String &name) const;
  int findEmotionIndexByPath(const String &path) const;

  String currentEmotion_;
  String previousEmotion_;
  String tiltUpEmotion_;
  String tiltSideEmotion_;
  std::vector<EmotionDefinition> emotionDefinitions_;
};

#endif // EMOTION_STATE_HPP
