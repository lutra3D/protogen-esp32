#ifndef COLOR_HPP
#define COLOR_HPP

 #include <Arduino.h>

struct Color {
  public:
    uint8_t getRed() const;
    uint8_t getGreen() const;
    uint8_t getBlue() const;

    explicit Color(uint8_t r, uint8_t g, uint8_t b);
    void set(uint8_t r, uint8_t g, uint8_t b);
    String toHexString() const;
    bool setFromHex(const String &hex);
  private:
    uint8_t red = 255;
    uint8_t green = 255;
    uint8_t blue = 255;
  };

  #endif //COLOR_HPP