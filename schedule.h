#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include <queue>
#include <memory>

#include "job.h"

struct Schedule {
  std::queue<std::unique_ptr<Job>> queue;

  void add_job(std::unique_ptr<Job>);
  void process();
};

#endif /* _SCHEDULE_H_ */