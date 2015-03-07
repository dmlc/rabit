/*!
 * \file timer.h
 * \brief This file defines the utils for timing
 * \author Tianqi Chen, Nacho, Tianyi
 */
#ifndef RABIT_TIMER_H
#define RABIT_TIMER_H
#include <time.h>
#include "./utils.h"

#ifdef __APPLE__
#include <mach/mach_time.h>
#define ORWL_NANO (+1.0E-9)
#define ORWL_GIGA UINT64_C(1000000000)

static double orwl_timebase = 0.0;
static uint64_t orwl_timestart = 0;
#endif  /* __APPLE__ */

namespace rabit {
namespace utils {
    
#ifdef __APPLE__
    /* OS X replacement coded by Jens Gustedt,
       http://stackoverflow.com/questions/5167269/clock-gettime-alternative-in-mac-os-x  */
    struct timespec clock_gettime(void) {
        if (!orwl_timestart) {
            mach_timebase_info_data_t tb = { 0 };
            mach_timebase_info(&tb);
            orwl_timebase = tb.numer;
            orwl_timebase /= tb.denom;
            orwl_timestart = mach_absolute_time();
        }
        struct timespec t;
        double diff = (mach_absolute_time() - orwl_timestart) * orwl_timebase;
        t.tv_sec = diff * ORWL_NANO;
        t.tv_nsec = diff - (t.tv_sec * ORWL_GIGA);
        return t;
    }
#endif

/*!
 * \brief return time in seconds, not cross platform, avoid to use this in most places
 */
inline double GetTime(void) {
  timespec ts;
#ifdef __APPLE__
    ts = clock_gettime();
#else
  utils::Check(clock_gettime(CLOCK_REALTIME, &ts) == 0, "failed to get time");
#endif
  return static_cast<double>(ts.tv_sec) + static_cast<double>(ts.tv_nsec) * 1e-9;
}
}
}
#endif
