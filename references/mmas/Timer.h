#ifndef TIMER_H
#define TIMER_H
#include <sys/time.h>
#include <sys/resource.h>

class Timer {
private:
  struct rusage res;
  struct timeval tp;
  double virtual_time, real_time;

public:
  enum TYPE {REAL, VIRTUAL};
  Timer(void);
  void resetTime();
  double elapsedTime(const TYPE& type);
};
#endif
