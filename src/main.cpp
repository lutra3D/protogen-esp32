#define MAIN
#ifdef MAIN
#include <Arduino.h>

#include "AbstractFaceDisplay.hpp"
#include "FaceDisplay.hpp"
#include "NeopixelFaceDisplay.hpp"
#include "FileManager.hpp"
#include "DisplayManager.hpp"
#include "EarController.hpp"
#include "EmotionState.hpp"
#include "FanController.hpp"
#include "SettingsStorage.hpp"
#include "TiltController.hpp"
#include "WebServerManager.hpp"
#include "BLEController.hpp"
#include "Capabilities/CapabilityManager.hpp"

// Display configuration
constexpr int PANEL_RES_X = 64;
constexpr int PANEL_RES_Y = 32;
constexpr int PANEL_CHAIN = 2;

// Wi-Fi configuration
const char *WIFI_NAME = "Proto";
const char *WIFI_PASS = "Test123456!";

// Fan configuration
constexpr uint8_t FAN_PWM_PIN = 32;
constexpr uint8_t FAN_PWM_CHANNEL = 0;
constexpr uint32_t FAN_PWM_FREQUENCY = 25000;
constexpr uint8_t FAN_PWM_RESOLUTION = 8;

// Ear LED configuration
constexpr uint16_t LEDS_PER_DISPLAY = 32;
constexpr uint8_t DATA_PIN_EARS = 33;

constexpr uint8_t PIN_SDA = 21;
constexpr uint8_t PIN_SCL = 22;

constexpr bool ALLOW_ALL_FILE_CHANGES = true;


EmotionState emotionState;
FanController fanController(FAN_PWM_PIN, FAN_PWM_CHANNEL, FAN_PWM_FREQUENCY, FAN_PWM_RESOLUTION);
EarController earController(LEDS_PER_DISPLAY, DATA_PIN_EARS);
TiltController tiltController(emotionState, PIN_SDA, PIN_SCL);
FileManager fileManager;
#if defined(FACE_NEOPIXEL_OUT_L) && defined(FACE_NEOPIXEL_OUT_R)
constexpr uint8_t FACE_NEOPIXEL_PANEL_WIDTH = 16;
constexpr uint8_t FACE_NEOPIXEL_PANEL_HEIGHT = 16;
NeopixelFaceDisplay faceDisplay(FACE_NEOPIXEL_PANEL_WIDTH, FACE_NEOPIXEL_PANEL_HEIGHT,
                                FACE_NEOPIXEL_OUT_L, FACE_NEOPIXEL_OUT_R);
#else
FaceDisplay faceDisplay(PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN);
#endif
SettingsStorage settingsStorage(emotionState, fanController, earController);
void onSettingsChanged()
{
  settingsStorage.save();
}
CapabilityManager capabilityManager(earController, fanController, onSettingsChanged);
WebServerManager webServerManager(emotionState, fanController, earController,
                                  tiltController, fileManager,
                                  capabilityManager,
                                  onSettingsChanged, 
                                  ALLOW_ALL_FILE_CHANGES);
DisplayManager displayManager(PIN_SDA, PIN_SCL, emotionState, fanController, earController);
BLEController bleController(emotionState, capabilityManager); 

void setup() {
  Serial.begin(115200);

  tiltController.begin();

  if (!fileManager.begin()) {
    while (true) {
      delay(1000);
    }
  }


  if (!faceDisplay.begin()) {
    while (true) {
      delay(1000);
    }
  }

  fileManager.printEmotions();

  fanController.begin();

  if (!earController.begin()) {
    Serial.println(F("[E] Starting LED driver failed"));
  }

  if (!settingsStorage.load())
  {
    Serial.println(F("[W] Continuing with default settings due to load error."));
  }

  webServerManager.begin(WIFI_NAME, WIFI_PASS);

  displayManager.begin();

  if(!bleController.begin()) {
      Serial.println(F("[E] An Error has occurred while starting BLE!"));
    }

  Serial.println(F("[I] Init done"));
}

void loop() {
  webServerManager.loop();
  tiltController.update();
  faceDisplay.playEmotion(emotionState.getCurrentEmotion());
  earController.update();
  displayManager.update();
}
#endif
