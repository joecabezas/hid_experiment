#ifndef _INTERPOLATORS_H_
#define _INTERPOLATORS_H_

#include <variant>

#include "point.h"

Point2D linear(Point2D p0, Point2D p1, float t);
Point2D idle(float t);

#endif /* _INTERPOLATORS_H_ */