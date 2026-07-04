#ifndef BLE_CONTROLLER_HPP
#define BLE_CONTROLLER_HPP

#include "NimBLEDevice.h"
#include "FileManager.hpp"
#include "EmotionState.hpp"
#include "EarController.hpp"
#include "Capabilities/CapabilityManager.hpp"

#include <Arduino.h>

class BLEController 
{
    public:
        BLEController(EmotionState &emotionState, CapabilityManager &capabilityManager, EarController &earController);
        bool begin();
        void update();
    private:
        BLEServer *pServer = NULL;
        BLECharacteristic * pCharacteristic;
        BLEAdvertising* pAdvertising;
        EmotionState &emotionState_;
        CapabilityManager &capabilityManager_;
        EarController &earController_;

        class CharacteristicCallbacks : public NimBLECharacteristicCallbacks {
            public:
                CharacteristicCallbacks(EmotionState &emotionState, CapabilityManager &capabilityManager, EarController &earController);
                void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override;

            private:
                EmotionState &emotionState_;
                CapabilityManager &capabilityManager_;
                EarController &earController_;
                void setEmotionAndApplyEars(const String &emotionPath);
        };
};

#endif
