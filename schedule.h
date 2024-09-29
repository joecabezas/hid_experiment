#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include <memory>
#include <queue>

#include "job.h"

struct Schedule {
  std::queue<std::unique_ptr<Job>> queue;

  void add_job(std::unique_ptr<Job> job);
  void add_job(uint8_t* buffer, size_t size);
  void process();
};

#endif /* _SCHEDULE_H_ */