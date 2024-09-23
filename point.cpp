#include "point.h"

Point2D Point2D::operator+(const Point2D& other) const {
  return Point2D{static_cast<int16_t>(x + other.x),
                 static_cast<int16_t>(y + other.y)};
}

Point2D Point2D::operator-(const Point2D& other) const {
  return Point2D{static_cast<int16_t>(x - other.x),
                 static_cast<int16_t>(y - other.y)};
}

Point2D Point2D::operator*(float scalar) const {
    return Point2D{
        static_cast<int16_t>(static_cast<float>(x) * scalar), 
        static_cast<int16_t>(static_cast<float>(y) * scalar)
    };
}