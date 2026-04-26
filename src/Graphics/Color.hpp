#ifndef COLOR_HPP
#define COLOR_HPP

 #include <Arduino.h>

struct Color {
  public:
    uint8_t getRed() const;
    uint8_t getGreen() const;
    uint8_t getBlue() const;

    explicit Color();
    Color(uint8_t r, uint8_t g, uint8_t b);
    Color(uint16_t color565);
    void set(uint8_t r, uint8_t g, uint8_t b);
    String toHexString() const;
    bool setFromHex(const String &hex);
  private:
    uint8_t expand5to8(uint8_t value) const { return (value << 3) | (value >> 2); }
    uint8_t expand6to8(uint8_t value) const { return (value << 2) | (value >> 4); }

    uint8_t red = 255;
    uint8_t green = 255;
    uint8_t blue = 255;
  };

  #endif //COLOR_HPP