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

#ifndef __ASM_ARCH_HARDWARE_H__
#define __ASM_ARCH_HARDWARE_H__

#include <linux/config.h>
#include <asm/arch/param.h>

#include <asm/arch/star_sys_memory_map.h>
#include <asm/arch/star_intc.h>
#include <asm/arch/star_timer.h>
#include <asm/arch/star_uart.h>
#include <asm/arch/star_gpio.h>
#include <asm/arch/star_pci_bridge.h>
#include <asm/arch/star_misc.h>
#include <asm/arch/star_powermgt.h>
//#include <asm/arch/star_usb.h>
#include <asm/arch/star_rtc.h>
//#include <asm/arch/star_watchdog.h>
#include <asm/arch/star_smc.h>
#include <asm/arch/star_gsw.h>
#include <asm/arch/star_ddrc.h>

#define SYS_CLK			(100000000) // 100MHz
#define UART_CLK		(48000000) // 48Mhz
#define AHB_CLK			(SYS_CLK)
#define APB_CLK			(AHB_CLK >> 1)
#define TIMER_COUNTER		(APB_CLK / HZ)

#define DEBUG_CONSOLE		(0)

#define FLASH_BASE_ADDR		SYSPA_FLASH_BASE_ADDR
#define FLASH_SIZE		0x800000

#define PCIBIOS_MIN_IO		0x00000000
#define PCIBIOS_MIN_MEM		0x00000000
#if 1
#define pcibios_assign_all_busses()	0
#else
#define pcibios_assign_all_busses()	1
#endif

#endif /* __ASM_ARCH_HARDWARE_H__ */
