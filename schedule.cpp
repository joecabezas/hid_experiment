#include "schedule.h"

#include "bsp/board_api.h"

void Schedule::add_job(std::unique_ptr<Job> job) {
  printf("schedule:add_job t0 queue size: %d\n", queue.size());
  queue.push(std::move(job));
  printf("schedule:add_job t1 queue size: %d\n", queue.size());
};

void Schedule::process() {
  while (!queue.empty() && queue.front()->status == FINISHED) {
    printf("schedule:process t0 status: %d\n", queue.front()->status);
    printf("schedule:process t0 queue size: %d\n", queue.size());
    queue.pop();
    printf("schedule:process t1 queue size: %d\n", queue.size());
  }
  if (queue.empty()) return;

  queue.front()->process();
}
