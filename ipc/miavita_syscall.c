#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/miavita_xtime.h>

struct timeval __miavitaxtime = {0, 0};

void init_miavita_xtime(void){
  do_gettimeofday(&__miavitaxtime);
  __miavitaxtime.tv_sec = 0;
}

void pulse_miavita_xtime(void){
  struct timeval t;
  do_gettimeofday(&t);
  __miavitaxtime.tv_usec = t.tv_usec;
  __miavitaxtime.tv_sec += 1;
}

/*
 * Mia-Vita's syscall to know microseconds from gps.
 * This must return the seconds with microsecond precision.
 */
asmlinkage int sys_miavitasyscall(void){
  struct timeval t;
  do_gettimeofday(&t);
  return __miavitaxtime.tv_sec * 1000000 + (t.tv_usec - __miavitaxtime.tv_usec) ;
}
