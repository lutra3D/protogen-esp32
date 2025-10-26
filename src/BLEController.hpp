#ifndef BLE_CONTROLLER_HPP
#define BLE_CONTROLLER_HPP

#include "NimBLEDevice.h"
#include "AnimationManager.hpp"
#include "EmotionState.hpp"

#include <Arduino.h>

class BLEController 
{
    public:
        BLEController(AnimationManager &animationManager, EmotionState &emotionState);
        bool begin();
        void update();
    private:
        BLEServer *pServer = NULL;
        BLECharacteristic * pCharacteristic;
        BLEAdvertising* pAdvertising;
        AnimationManager &animationManager_;
        EmotionState &emotionState_;

        class CharacteristicCallbacks : public NimBLECharacteristicCallbacks {
            public:
                CharacteristicCallbacks(AnimationManager &animationManager, EmotionState &emotionState);
                void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override;

            private:
                AnimationManager &animationManager_;
                EmotionState &emotionState_;
        };
};

#endif