#ifndef BLE_CONTROLLER_HPP
#define BLE_CONTROLLER_HPP

#include "NimBLEDevice.h"
#include "FileManager.hpp"
#include "EmotionState.hpp"

#include <Arduino.h>

class BLEController 
{
    public:
        BLEController(FileManager &fileManager, EmotionState &emotionState);
        bool begin();
        void update();
    private:
        BLEServer *pServer = NULL;
        BLECharacteristic * pCharacteristic;
        BLEAdvertising* pAdvertising;
        FileManager &fileManager_;
        EmotionState &emotionState_;

        class CharacteristicCallbacks : public NimBLECharacteristicCallbacks {
            public:
                CharacteristicCallbacks(FileManager &fileManager, EmotionState &emotionState);
                void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override;

            private:
                FileManager &fileManager_;
                EmotionState &emotionState_;
        };
};

#endif