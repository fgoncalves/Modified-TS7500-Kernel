/*******************************************************************************
 *
 *  Copyright (c) 2008 Cavium Networks 
 * 
 *  This file is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License, Version 2, as 
 *  published by the Free Software Foundation. 
 *
 *  This file is distributed in the hope that it will be useful, 
 *  but AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or 
 *  NONINFRINGEMENT.  See the GNU General Public License for more details. 
 *
 *  You should have received a copy of the GNU General Public License 
 *  along with this file; if not, write to the Free Software 
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA or 
 *  visit http://www.gnu.org/licenses/. 
 *
 *  This file may also be available under a different license from Cavium. 
 *  Contact Cavium Networks for more information
 *
 ******************************************************************************/

#include <linux/config.h>
#include <linux/linkage.h>
#include <asm/hardware.h>

		.macro	addruart,rx
		mrc	p15, 0, \rx, c1, c0
		tst	\rx, #1				@ MMU enabled ?
		moveq	\rx, #(SYSPA_UART0_BASE_ADDR)	@ physical base address of UART0
		movne	\rx, #(SYSVA_UART0_BASE_ADDR & 0xFF000000) @ virtual base address of UART0
		orrne	\rx, \rx, #(SYSVA_UART0_BASE_ADDR & 0x00FF0000)
		orrne	\rx, \rx, #(SYSVA_UART0_BASE_ADDR & 0x0000FF00)
		orrne	\rx, \rx, #(SYSVA_UART0_BASE_ADDR & 0x000000FF)
		.endm

		.macro	senduart,rd,rx
		strb	\rd, [\rx, #0x00]
		.endm

		.macro	waituart,rd,rx
		mov	\rd, #0xf000
1001:		subs	\rd, \rd, #1
		bne	1001b	
		.endm

		.macro	busyuart,rd,rx
		nop
		mov	\rd, #0xf000
1010:		subs	\rd, \rd, #1
		bne	1010b
		.endm

