#include "Color.hpp"

Color::Color() : red(0), green(0), blue(0) {}

Color::Color(uint8_t r, uint8_t g, uint8_t b)
{
    red = r;
    green = g;
    blue = b;
}

Color::Color(uint16_t color565)
{
    red = expand5to8((color565 >> 11) & 0x1F);
    green = expand6to8((color565 >> 5) & 0x3F);
    blue = expand5to8(color565 & 0x1F);
}

void Color::set(uint8_t r, uint8_t g, uint8_t b)
{
    red = r;
    green = g;
    blue = b;
}

uint8_t Color::getRed() const
{
    return red;
}

uint8_t Color::getGreen() const
{
    return green;
};

uint8_t Color::getBlue() const
{
    return blue;
};

String Color::toHexString() const
{
    char buffer[8];
    snprintf(buffer, sizeof(buffer), "#%02x%02x%02x", red, green, blue);
    return String(buffer);
}

bool Color::setFromHex(const String &hex)
{
    if (hex.length() != 7 || hex.charAt(0) != '#')
    {
        return false;
    }

    auto parseComponent = [&hex](uint8_t start) -> int
    {
        String component = hex.substring(start, start + 2);
        long value = strtol(component.c_str(), nullptr, 16);
        if (value < 0 || value > 255)
        {
            return -1;
        }
        return static_cast<int>(value);
    };

    const int r = parseComponent(1);
    const int g = parseComponent(3);
    const int b = parseComponent(5);
    if (r < 0 || g < 0 || b < 0)
    {
        return false;
    }

    set(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b));
    return true;
}