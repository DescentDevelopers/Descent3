#ifndef SB_TIME_H
#define SB_TIME_H
#include <stdint.h>
typedef long long time_t;
struct timespec { time_t tv_sec; long tv_nsec; };
#define CLOCK_MONOTONIC 1
int clock_gettime(int clk, struct timespec *ts);
#endif
