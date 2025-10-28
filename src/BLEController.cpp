#include "BLEController.hpp"
#include "NimBLEDevice.h"

BLEController::CharacteristicCallbacks::CharacteristicCallbacks(AnimationManager &animationManager, EmotionState &emotionState)
    : animationManager_(animationManager),
      emotionState_(emotionState)
{
}

void BLEController::CharacteristicCallbacks::onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo)
{
    auto emotions = animationManager_.getEmotions();
    auto emotionCount = emotions.size();

    String characteristicValue = String(pCharacteristic->getValue().c_str());

    Serial.println("[I] BT Received: " + String(characteristicValue));

    if (characteristicValue.charAt(0) == 'g')
    { // legacy remote reasons
        pCharacteristic->setValue("i" + String(emotionCount));
        pCharacteristic->notify();
        return;
    }

    if (characteristicValue.charAt(0) == '?')
    {
        String availableEmotionsMessage;
        for (int i = 0; i < emotionCount; i++)
        {
            auto emotion = emotions[i];
            availableEmotionsMessage += emotion.name;
            availableEmotionsMessage += ";";
        }
        pCharacteristic->setValue(availableEmotionsMessage);
        pCharacteristic->notify(true);
        return;
    }

    if (characteristicValue.charAt(0) == ';')
    { // command
        if (characteristicValue.indexOf("rgb") > 0)
        {
            //TODO: Use this for ear color
        }
        else if (characteristicValue.indexOf("set") > 0)
        {
            //TODO: Use this for ear brightness
        }
        return;
    }

    if (characteristicValue.toInt() > 0 && characteristicValue.toInt() <= emotionCount)
    { // legacy remote reasons
        auto newEmotion = emotions[characteristicValue.toInt() - 1];
        emotionState_.setCurrentEmotion(newEmotion.name);
        return;
    }
    
    for (int i = 0; i < emotionCount; i++)
    {
        auto emotion = emotions[i];
        if (characteristicValue == emotion.name)
        {
            emotionState_.setCurrentEmotion(emotion.name);
        }
    } 
}

class ServerCallbacks : public NimBLEServerCallbacks
{
    void onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo, int reason) override
    {
        NimBLEDevice::startAdvertising();
    }
} serverCallbacks;

BLEController::BLEController(AnimationManager &animationManager, EmotionState &emotionState)
    : animationManager_(animationManager),
    emotionState_(emotionState)
{
}

bool BLEController::begin()
{
    auto emotions = animationManager_.getEmotions();
    std::string stdStr("Proto", 5);
    BLEDevice::init(stdStr);
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);

    pServer = BLEDevice::createServer();
    pServer->setCallbacks(&serverCallbacks);

    BLEService *pService = pServer->createService("ffe0");

    pCharacteristic = pService->createCharacteristic("ffe1",
                                                     NIMBLE_PROPERTY::BROADCAST | NIMBLE_PROPERTY::READ |
                                                         NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::WRITE |
                                                         NIMBLE_PROPERTY::INDICATE);
    pCharacteristic->setValue(emotions.size());

    auto chrCallbacks = new CharacteristicCallbacks(animationManager_, emotionState_);

    pCharacteristic->setCallbacks(chrCallbacks);

    if (!pService->start())
    {
        return false;
    }

    pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->setName(stdStr);
    pAdvertising->addServiceUUID(BLEUUID(pService->getUUID()));
    pAdvertising->enableScanResponse(true);
    if (!pAdvertising->start(0))
    {
        return false;
    }

    return true;
}