#include "bsp/board_api.h"
#include "linear_job.h"
#include "usb_descriptors.h"


void Job::start() {
  printf("starting job!\n");
  assert(status == SCHEDULED);
  start_time = board_millis();
  status = STARTED;
};

void Job::finish() { status = FINISHED; };
