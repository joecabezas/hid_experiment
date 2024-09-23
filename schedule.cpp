#include "schedule.h"

#include "bsp/board_api.h"

void Schedule::add_job(Job job) { queue.push(job); };

void Schedule::process() {
  while (!queue.empty() && queue.front().status == FINISHED) queue.pop();
  if (queue.empty()) return;

  queue.front().process();
}
