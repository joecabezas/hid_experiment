#ifndef _LINEAR_JOB_H_
#define _LINEAR_JOB_H_

#include "job.h"

class LinearJob : public Job {
 public:
  Point2D p0;
  Point2D p1;

  LinearJob(Point2D _p0, Point2D _p1, uint32_t _duration) : p0(_p0), p1(_p1) {
    duration = _duration;
  }

  void process() override;
};

#endif /* _LINEAR_JOB_H_ */