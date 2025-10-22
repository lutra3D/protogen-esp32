#define MAIN
#ifdef MAIN
#include <Arduino.h>

#include "AnimationManager.hpp"
#include "DisplayManager.hpp"
#include "EarController.hpp"
#include "EmotionState.hpp"
#include "FanController.hpp"
#include "TiltController.hpp"
#include "WebServerManager.hpp"

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
constexpr uint8_t PIN_SCL = 2;

EmotionState emotionState;
FanController fanController(FAN_PWM_PIN, FAN_PWM_CHANNEL, FAN_PWM_FREQUENCY, FAN_PWM_RESOLUTION);
EarController earController(LEDS_PER_DISPLAY, DATA_PIN_EARS);
TiltController tiltController(emotionState, PIN_SDA, PIN_SCL);
AnimationManager animationManager(PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN);
WebServerManager webServerManager(emotionState, fanController, earController, tiltController);
DisplayManager displayManager(PIN_SDA, PIN_SCL, emotionState, fanController, earController);

void setup() {
  Serial.begin(115200);

  // tiltController.begin();

  if (!animationManager.begin()) {
    while (true) {
      delay(1000);
    }
  }

  fanController.begin();

  if (!earController.begin()) {
    Serial.println(F("Starting LED driver failed"));
  }

  webServerManager.begin(WIFI_NAME, WIFI_PASS);

  displayManager.begin();

  Serial.println(F("Init done"));
}

void loop() {
  webServerManager.loop();
  // tiltController.update();
  animationManager.playEmotion(emotionState.getCurrentEmotion());
  earController.update();
  displayManager.update();
}
#endif
