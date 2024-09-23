#ifndef _POINT_H_
#define _POINT_H_

#include <cstdint>

struct Point2D {
  int16_t x;
  int16_t y;

  Point2D operator+(const Point2D& other) const;
  Point2D operator-(const Point2D& other) const;
  Point2D operator*(float scalar) const;
};

#endif /* _POINT_H */