#ifndef _JOB_H_
#define _JOB_H_

#include "interpolation_strategies.h"
#include "point.h"

enum JobStatus { SCHEDULED, STARTED, FINISHED };

enum JobStrategy {
  LINEAR,
};

// Function pointer definition for strategy functions
typedef Point2D (*StrategyFunction)(StrategyArgsVariant strategy_args, float t);

struct Job {
  JobStatus status;
  uint32_t start_time;
  uint32_t duration;

  JobStrategy strategy;
  StrategyArgsVariant strategy_args;

  void process();
  void start();
  void finish();
  StrategyFunction get_strategy();
  StrategyFunction function;

  Job(JobStrategy strat, StrategyArgsVariant args, uint32_t dur)
      : status(SCHEDULED),
        strategy(strat),
        strategy_args(args),
        duration(dur),
        function(nullptr) {}
};

#endif /* _JOB_H_ */