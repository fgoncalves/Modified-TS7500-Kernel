#ifndef __MIAVITA_XTIME_H__
#define __MIAVITA_XTIME_H__

#include <linux/time.h>

extern uint64_t __miavita_elapsed_secs, __miavita_elapsed_usecs;

extern void init_miavita_xtime(void);
extern void pulse_miavita_xtime(void);
#endif
