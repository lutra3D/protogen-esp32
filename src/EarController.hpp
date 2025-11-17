#ifndef EAR_CONTROLLER_HPP
#define EAR_CONTROLLER_HPP

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <ArduinoJson.h>

class Ear {
public:
  struct Color {
    uint8_t red = 255;
    uint8_t green = 255;
    uint8_t blue = 255;

    void set(uint8_t r, uint8_t g, uint8_t b);
    String toHexString() const;
    bool setFromHex(const String &hex);
  };

  class Brightness {
  public:
    explicit Brightness(uint8_t value = 80);

    void setValue(uint8_t value);
    uint8_t getValue() const;
    void setPercent(float percent);
    float getPercent() const;

  private:
    uint8_t value_;
  };

  Ear();

  void setColor(uint8_t red, uint8_t green, uint8_t blue);
  bool setColorFromHex(const String &hex);
  const Color &getColor() const;
  String getColorHexString() const;

  void setBrightness(uint8_t brightness);
  void setBrightnessPercent(float percent);
  bool setBrightnessPercentChecked(float percent);
  uint8_t getBrightness() const;
  float getBrightnessPercent() const;

  void serialize(JsonVariant json) const;

private:
  Color color_;
  Brightness brightness_;
};

class EarController {
public:
  EarController(uint16_t ledCount, uint8_t dataPin);

  bool begin();
  void setBrightness(uint8_t brightness);
  void setBrightnessPercent(float percent);
  uint8_t getBrightness() const;

  void setColor(uint8_t red, uint8_t green, uint8_t blue);
  uint8_t getRed() const;
  uint8_t getGreen() const;
  uint8_t getBlue() const;

  String getColorHexString() const;
  float getBrightnessPercent() const;

  Ear &getEar();
  const Ear &getEar() const;

  void update();

private:
  uint16_t ledCount_;
  Adafruit_NeoPixel earLeds_;
  Ear ear_;
};

#endif // EAR_CONTROLLER_HPP
