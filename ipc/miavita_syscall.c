#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/miavita_xtime.h>
/*
 * Mia-Vita's syscall to know microseconds from gps.
 * This must return the seconds with microsecond precision.
 */
asmlinkage int sys_miavitasyscall(void){
  struct timeval t;
  do_gettimeofday(&t);
  return __miavitaxtime.tv_sec * 1000000 + (t.tv_usec - __miavitaxtime.tv_usec) ;
}
