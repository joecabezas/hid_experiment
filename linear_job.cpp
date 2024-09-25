#include "linear_job.h"

#include "bsp/board_api.h"
#include "usb_descriptors.h"


void LinearJob::process() {
  printf("process\n");
  printf("status:%d\n", status);
  if (status == SCHEDULED) start();

  printf("status after processing: %d\n", status);

  uint32_t millis = board_millis();
  uint32_t delta_time = millis - start_time;

  float t = static_cast<float>(delta_time) / static_cast<float>(duration);
  printf("delta_time:%d\n", delta_time);
  printf("duration:%d\n", duration);
  printf("t:%f\n", t);
  if (t > 1.0f) {
    t = 1.0f;
    finish();
  };

  printf("function 1: args.p0.x:%d, args.p0.y:%d\n", p0.x, p0.y);
  printf("function 1: args.p1.x:%d, args.p1.y:%d\n", p1.x, p1.y);
  printf("function 1: t:%f\n", t);
  Point2D p = linear(p0, p1, t);
  p.x *= 9;
  p.y *= 16;

  printf("moving mouse to: (%d, %d)\n", p.x, p.y);
  tud_hid_abs_mouse_report(REPORT_ID_MOUSE, 0x00, (uint16_t)p.x, (uint16_t)p.y,
                           0x80, 0x80);
};