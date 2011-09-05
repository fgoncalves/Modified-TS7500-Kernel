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

#include <asm/arch/star_intc.h>

	.macro	disable_fiq
	.endm

	.macro	get_irqnr_preamble, base, tmp
	.endm

	.macro	arch_ret_to_user, tmp1, tmp2
	.endm


#ifdef CONFIG_VIC_INTERRUPT
	.macro	get_fiqnr_and_base, irqnr, irqstat, base, tmp
	ldr	\base, =(SYSVA_VIC_BASE_ADDR + 0x140)
	ldr	\irqnr, [\base]
	.endm

	.macro	get_irqnr_and_base, irqnr, irqstat, base, tmp
	ldr	\base, =(SYSVA_VIC_BASE_ADDR + 0x140)
	ldr	\irqnr, [\base]
	.endm
#else
	.macro	get_fiqnr_and_base, irqnr, irqstat, base, tmp
	ldr	\base, =(SYSVA_VIC_BASE_ADDR + 0x20)
	ldr	\irqstat, [\base]
	mov	\irqnr, #0
9001:
	tst	\irqstat, #1
	bne	9002f
	add	\irqnr, \irqnr, #1
	mov	\irqstat, \irqstat, lsr #1
	cmp	\irqnr, #32
	bcc	9001b
9002:
	.endm

	.macro	get_irqnr_and_base, irqnr, irqstat, base, tmp
	ldr	\base, =(SYSVA_VIC_BASE_ADDR + 0x1C)
	ldr	\irqstat, [\base]
	mov	\irqnr, #0
9003:
	tst	\irqstat, #1
	bne	9004f
	add	\irqnr, \irqnr, #1
	mov	\irqstat, \irqstat, lsr #1
	cmp	\irqnr, #32
	bcc	9003b
9004:
	.endm
#endif

	.macro	irq_prio_table
	.endm

