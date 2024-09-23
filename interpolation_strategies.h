#ifndef _MOUSE_STRATEGIES_H
#define _MOUSE_STRATEGIES_H

#include <variant>

#include "point.h"

struct MoveToLinearArgs {
  Point2D p0;
  Point2D p1;
};

using StrategyArgsVariant = std::variant<MoveToLinearArgs>;

Point2D moveToLinear(StrategyArgsVariant args, float t);

#endif /* _MOUSE_STRATEGIES_H */