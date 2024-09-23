#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include <queue>

#include "job.h"

struct Schedule {
  std::queue<Job> queue;

  void add_job(Job);
  void process();
};

#endif /* _SCHEDULE_H_ */