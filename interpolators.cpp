#include "interpolators.h"

#include <stdio.h>

Point2D linear(Point2D p0, Point2D p1, float t) {
  Point2D delta = p1 - p0;
  return p0 + (delta * t);
}

Point2D idle(float t) { return Point2D{40, 40}; };