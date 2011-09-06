#ifndef __MIAVITA_XTIME_H__
#define __MIAVITA_XTIME_H__

#include <linux/time.h>

extern struct timeval __miavitaxtime;

extern void init_miavita_xtime(void);
extern void pulse_miavita_xtime(void);
#endif
