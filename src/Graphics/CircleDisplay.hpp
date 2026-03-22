#ifndef CIRCLE_DISPLAY_HPP
#define CIRCLE_DISPLAY_HPP

struct RasterPoint
{
  float x;
  float y;
};


class CircleDisplay
{
public:
    RasterPoint getRasterPoint(int index, int length) const;
private:
    static constexpr int kLedsPerCircle = 16;
    static constexpr float kCircleSpacing = 4.0f;

    RasterPoint getLocalCirclePoint(int ledIndex) const;
};

#endif // CIRCLE_DISPLAY_HPP