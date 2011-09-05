/* $Id: reg_rdwr.h,v 1.1.1.1 2010/03/11 21:10:23 kris Exp $
 *
 * Read/write register macros used by *_defs.h
 */

#ifndef reg_rdwr_h
#define reg_rdwr_h


#define REG_READ(type, addr) *((volatile type *) (addr))

#define REG_WRITE(type, addr, val) \
   do { *((volatile type *) (addr)) = (val); } while(0)

#endif
