#ifndef _JOB_H_
#define _JOB_H_

#include "interpolators.h"
#include "point.h"

enum JobStatus { SCHEDULED, STARTED, FINISHED };
enum JobStrategy { LINEAR, IDLE };

class Job {
 public:
  JobStatus status = SCHEDULED;
  uint32_t start_time;
  uint32_t duration;

  void start();
  void finish();
  virtual void process() = 0;
};

#endif /* _JOB_H_ */