#include "schedule.h"

#include "bsp/board_api.h"
#include "linear_job.h"
#include "network.h"
#include "pico/stdlib.h"

void Schedule::process() {
  while (!queue.empty() && queue.front()->status == FINISHED) {
    printf("schedule:process t0 status: %d\n", queue.front()->status);
    printf("schedule:process t0 queue size: %d\n", queue.size());
    queue.pop();
    printf("schedule:process t1 queue size: %d\n", queue.size());
  }
  if (queue.empty()) return;

  queue.front()->process();
};

void Schedule::add_job(std::unique_ptr<Job> job) {
  printf("schedule:add_job t0 queue size: %d\n", queue.size());
  queue.push(std::move(job));
  printf("schedule:add_job t1 queue size: %d\n", queue.size());
};

void Schedule::add_job(uint8_t* buffer, size_t size) {
  NetworkCommand* cmd = new NetworkCommand;
  bool status;
  decode_message(buffer, size, &cmd, &status);

  printf("cmd: %d\n", cmd->cmd);
  if (cmd->cmd == NetworkCommandType_LINEAR) {
    Point2D p0 = {
        static_cast<int16_t>(cmd->args.pair.p0.x),
        static_cast<int16_t>(cmd->args.pair.p0.y),
    };
    Point2D p1 = {
        static_cast<int16_t>(cmd->args.pair.p1.x),
        static_cast<int16_t>(cmd->args.pair.p1.y),
    };
    std::unique_ptr<Job> job = std::make_unique<LinearJob>(p0, p1, cmd->duration);
    printf("scheduling job...\n");
    add_job(std::move(job));
  }
};