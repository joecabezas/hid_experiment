#include "job.h"

#include "bsp/board_api.h"
#include "interpolation_strategies.h"
#include "usb_descriptors.h"

void Job::start() {
  printf("starting job!\n");
  assert(status == SCHEDULED);
  start_time = board_millis();
  status = STARTED;
};

void Job::finish() { status = FINISHED; };

void Job::process() {
  printf("job:process\n");
  printf("status:%d\n", status);
  if (status == SCHEDULED) start();

  printf("status after processing: %d\n", status);  // Check if status is updated

  uint32_t millis = board_millis();
  uint32_t delta_time = millis - start_time;

  if (function == nullptr) function = get_strategy();

  float t = static_cast<float>(delta_time) / static_cast<float>(duration);
  printf("delta_time:%d\n", delta_time);
  printf("duration:%d\n", duration);
  printf("t:%f\n", t);
  if (t > 1.0f) {
    t = 1.0f;
    finish();
  };

  if (function == nullptr) printf("function is NULL");

  printf("function 0\n");
  MoveToLinearArgs args = std::get<MoveToLinearArgs>(strategy_args);
  printf("function 1\n");
  printf("function 1: args.p0.x:%d, args.p0.y:%d\n", args.p0.x, args.p0.y);
  printf("function 1: args.p1.x:%d, args.p1.y:%d\n", args.p1.x, args.p1.y);
  printf("function 1: t:%f\n", t);
  Point2D p = function(strategy_args, t);
  printf("function 2\n");
  p.x *= 9;
  p.y *= 16;

  printf("moving mouse to: (%d, %d)\n", p.x, p.y);
  tud_hid_abs_mouse_report(REPORT_ID_MOUSE, 0x00, (uint16_t)p.x, (uint16_t)p.y,
                           0x80, 0x80);
};

StrategyFunction Job::get_strategy() {
  switch (strategy) {
    case LINEAR:
      printf("returning LINEAR\n");
      return moveToLinear;

    default:
      printf("error: nonexistent strategy %d\n", strategy);
      return nullptr;
  }
}