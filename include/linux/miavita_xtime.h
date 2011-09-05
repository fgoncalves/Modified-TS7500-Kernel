#ifndef __MIAVITA_XTIME_H__
#define __MIAVITA_XTIME_H__

#include <linux/time.h>

struct timeval __miavitaxtime = {0, 0};

static inline void init_miavita_xtime(void){
  do_gettimeofday(&__miavitaxtime);
  __miavitaxtime.tv_sec = 0;
}

static inline void pulse_miavita_xtime(void){
  struct timeval t;
  do_gettimeofday(&t);
  __miavitaxtime.tv_usec = t.tv_usec;
  __miavitaxtime.tv_sec += 1;
}
#endif
