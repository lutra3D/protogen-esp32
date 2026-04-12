#ifndef ANIMATION_MANAGER_HPP
#define ANIMATION_MANAGER_HPP

#include <Arduino.h>
#include <vector>

#include "FaceDisplay.hpp"
#include "FileManager.hpp"
#include "Model/AnimationFile.hpp"

class AnimationManager {
public:
  AnimationManager(int panelResX, int panelResY, int panelChainLength);

  bool begin();
  void playEmotion(const String &emotionPath);
  std::vector<AnimationFile> getEmotions() const;
  void printEmotions() const;

private:
  FileManager fileManager_;
  FaceDisplay faceDisplay_;
};

#endif // ANIMATION_MANAGER_HPP
