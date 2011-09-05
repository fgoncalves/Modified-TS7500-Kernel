/*******************************************************************************
 *
 *  Copyright(c) 2006 Star Semiconductor Corporation, All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along with
 *  this program; if not, write to the Free Software Foundation, Inc., 59
 *  Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *  The full GNU General Public License is included in this distribution in the
 *  file called LICENSE.
 *
 *  Contact Information:
 *  Technology Support <tech@starsemi.com>
 *  Star Semiconductor 4F, No.1, Chin-Shan 8th St, Hsin-Chu,300 Taiwan, R.O.C
 *
 ******************************************************************************/

#ifndef __ASM_ARCH_UNCOMPRESS_H__
#define __ASM_ARCH_UNCOMPRESS_H__

#include <asm/arch/star_uart.h>

static void putstr(const char *s)
{
	while (*s) {
		volatile unsigned int status = 0;

		do {
			status = __UART_LSR;
		} while (!((status & THR_EMPTY) == THR_EMPTY));

		__UART_THR = *s;

		if (*s == '\n') {
			do {
				status = __UART_LSR;
			} while (!((status & THR_EMPTY) == THR_EMPTY));
			__UART_THR = '\r';
		}
		s++;
	}
}

#define arch_decomp_setup()
#define arch_decomp_wdog()

#endif
