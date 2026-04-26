#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <Arduino.h>

// Wi-Fi configuration
const char *WIFI_NAME = "Proto";
const char *WIFI_PASS = "Test123456!";

// HUB75 P3 Display configuration
#define PANEL_RES_X 64
#define PANEL_RES_Y 32
#define PANEL_CHAIN 2

// Neopixel matrix configuration
// #define FACE_NEOPIXEL_PANEL_WIDTH 16 // Width of each Neopixel matrix
// #define FACE_NEOPIXEL_PANEL_HEIGHT 16 // Height of each Neopixel matrix
// #define FACE_NEOPIXEL_OUT_L 33 // GPIO pin for left Neopixel panel must support PWM
// #define FACE_NEOPIXEL_OUT_R 32 // GPIO pin for right Neopixel panel must support PWM

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

#endif // CONFIG_HPP