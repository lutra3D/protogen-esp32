#ifndef EMOTION_STATE_HPP
#define EMOTION_STATE_HPP

#include <Arduino.h>

#include <vector>

#include "float_helper.hpp"

class EmotionState {
public:
  struct EarGradient {
    String fromColor = "#ffffff";
    String toColor = "#ffffff";
    float directionX = 1.0f;
    float directionY = 0.0f;
    float midpoint = 0.5f;
  };

  struct EarColor {
    enum class Type {
      Solid,
      Gradient,
    };

    Type type = Type::Solid;
    String solidColor = "#ffffff";
    EarGradient gradient;
  };

  struct EmotionDefinition {
    String name;
    String path;
    EarColor earColor;
  };

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
