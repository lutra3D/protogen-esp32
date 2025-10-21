#define MAIN
#ifdef MAIN
#include <Arduino.h>

#include "AnimationManager.hpp"
#include "EarController.hpp"
#include "EmotionState.hpp"
#include "FanController.hpp"
#include "TiltController.hpp"
#include "WebServerManager.hpp"

#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

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

#define PIN_SDA 21
#define PIN_SCL 2

Adafruit_SH1106 display(PIN_SDA, PIN_SCL);

EmotionState emotionState;
FanController fanController(FAN_PWM_PIN, FAN_PWM_CHANNEL, FAN_PWM_FREQUENCY, FAN_PWM_RESOLUTION);
EarController earController(LEDS_PER_DISPLAY, DATA_PIN_EARS);
TiltController tiltController(emotionState, PIN_SDA, PIN_SCL);
AnimationManager animationManager(PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN);
WebServerManager webServerManager(emotionState, fanController, earController, tiltController);

void setup() {
  Serial.begin(115200);

  //tiltController.begin();

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

  display.begin(SH1106_EXTERNALVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.setRotation(2);
  display.println("Hello, world! How long of a thing I can write here? Let's try!");
  display.display();
  Serial.printf("Done!");

  Serial.println(F("Init done"));
}

void loop() {
  webServerManager.loop();
  //tiltController.update();
  animationManager.playEmotion(emotionState.getCurrentEmotion());
  earController.update();

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Hello, world! How long of a thing I can write here? Let's try!");
  display.display();
}
#endif