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
    CircleDisplay(int ledsPerCircle, float radius);
    RasterPoint getRasterPoint(int index, int length) const;
private:
    const int ledsPerCircle;
    const float radius;

    RasterPoint getLocalCirclePoint(int ledIndex) const;
};

#endif // CIRCLE_DISPLAY_HPP