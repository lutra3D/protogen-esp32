#include "BLEController.hpp"
#include "NimBLEDevice.h"

BLEController::CharacteristicCallbacks::CharacteristicCallbacks(EmotionState &emotionState, CapabilityManager &capabilityManager)
    : emotionState_(emotionState), capabilityManager_(capabilityManager)
{
}

void BLEController::CharacteristicCallbacks::onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo)
{
    auto emotions = emotionState_.getEmotionDefinitions();
    auto emotionCount = emotions.size();

    String characteristicValue = String(pCharacteristic->getValue().c_str());

    Serial.println("[I] BT Received: " + String(characteristicValue));

    if (characteristicValue.length() == 0)
    {
        return;
    }

    if (characteristicValue.charAt(0) == 'g')
    { // legacy remote reasons
        pCharacteristic->setValue("i" + String(emotionCount));
        pCharacteristic->notify();
        return;
    }

    if (characteristicValue.charAt(0) == '?')
    {
        auto capabilities = capabilityManager_.getAvailableCapabilities();
        String availableEmotionsMessage;
        String availableCapabilitiesMessage;

        for (int i = 0; i < emotionCount; i++)
        {
            auto emotion = emotions[i];
            availableEmotionsMessage += emotion.name;
            availableEmotionsMessage += ";";
        }

        for (const auto &capability : capabilities)
        {
            availableCapabilitiesMessage += capability;
            availableCapabilitiesMessage += ";";
        }

        if (characteristicValue == "?cap")
        {
            pCharacteristic->setValue(availableCapabilitiesMessage);
            pCharacteristic->notify(true);
            return;
        }

        if (characteristicValue == "?all")
        {
            pCharacteristic->setValue("E:" + availableEmotionsMessage + "|C:" + availableCapabilitiesMessage);
            pCharacteristic->notify(true);
            return;
        }

        pCharacteristic->setValue(availableEmotionsMessage);
        pCharacteristic->notify(true);
        return;
    }

    if (characteristicValue.charAt(0) == ';')
    { // command
        if (characteristicValue.startsWith(";cap:"))
        {
            auto capabilityName = characteristicValue.substring(5);
            auto capability = capabilityManager_.getCapabilityByName(capabilityName);
            if (capability == nullptr)
            {
                Serial.println("[W] Unknown capability: " + capabilityName);
                return;
            }

            capability->handle();
        }
        else if (characteristicValue.indexOf("rgb") > 0)
        {
            //TODO: Use this for ear color
        }
        else if (characteristicValue.indexOf("set") > 0)
        {
            //TODO: Use this for ear brightness
        }
        else
        {
            auto emotionName = characteristicValue.substring(1);
            for (int i = 0; i < emotionCount; i++)
            {
                auto emotion = emotions[i];
                if (emotionName == emotion.name)
                {
                    Serial.println("[I] Setting emotion" + emotion.path);
                    emotionState_.setCurrentEmotion(emotion.path);
                    return;
                }
            }
        }
        return;
    }

    if (characteristicValue.toInt() > 0 && characteristicValue.toInt() <= emotionCount)
    { // legacy remote reasons
        auto newEmotion = emotions[characteristicValue.toInt() - 1];
        Serial.println("[I] Setting emotion" + newEmotion.path);
        emotionState_.setCurrentEmotion(newEmotion.path);
        return;
    }
    
    for (int i = 0; i < emotionCount; i++)
    {
        auto emotion = emotions[i];
        if (characteristicValue == emotion.name)
        {
            Serial.println("[I] Setting emotion" + emotion.path);
            emotionState_.setCurrentEmotion(emotion.path);
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

BLEController::BLEController(EmotionState &emotionState, CapabilityManager &capabilityManager)
  :  emotionState_(emotionState), capabilityManager_(capabilityManager)
{
}

bool BLEController::begin()
{
    auto emotions = emotionState_.getEmotionDefinitions();
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

    auto chrCallbacks = new CharacteristicCallbacks(emotionState_, capabilityManager_);

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
