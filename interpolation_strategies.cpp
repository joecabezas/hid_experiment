#include "interpolation_strategies.h"
#include <stdio.h>

#define STEPS 30

Point2D moveToLinear(StrategyArgsVariant _args, float t) {
  printf("moveToLinear\n");
  MoveToLinearArgs args = std::get<MoveToLinearArgs>(_args);
  Point2D p0 = args.p0;
  Point2D p1 = args.p1;

  Point2D delta = p1 - p0;
  return p0 + (delta * t);
}