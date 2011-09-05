/*******************************************************************************
 *
 *
 *   Copyright(c) 2003 - 2006 Star Semiconductor Corporation, All rights reserved.
 *
 *   This program is free software; you can redistribute it and/or modify it
 *   under the terms of the GNU General Public License as published by the Free
 *   Software Foundation; either version 2 of the License, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful, but WITHOUT
 *   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *   more details.
 *
 *   You should have received a copy of the GNU General Public License along with
 *   this program; if not, write to the Free Software Foundation, Inc., 59
 *   Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *   The full GNU General Public License is included in this distribution in the
 *   file called LICENSE.
 *
 *   Contact Information:
 *   Technology Support <tech@starsemi.com>
 *   Star Semiconductor 4F, No.1, Chin-Shan 8th St, Hsin-Chu,300 Taiwan, R.O.C
 *
 ********************************************************************************/

// This macro or function divide two part, 
// one is initial state, another is in netdev open (ifconfig up) function.

#ifndef  DORADO2_H
#define  DORADO2_H

#include <linux/types.h>
#include "star_gsw.h"

// this configure is for star dorado2

// add by descent 2006/07/10
#define DORADO2

// add by KC 2006/09/07
#ifdef CONFIG_STAR9100_SHNAT_PCI_FASTPATH
// if no VSC8201 on MAC PORT1, we need to define this
// if VSC8201 is present, mark it
//#define DORADO2_PCI_FASTPATH_MAC_PORT1_LOOPBACK
#endif

#ifdef DORADO2
// init phy or switch chip
#define INIT_PORT0_PHY star_gsw_config_VSC7385();
#ifdef DORADO2_PCI_FASTPATH_MAC_PORT1_LOOPBACK
#define INIT_PORT1_PHY star_gsw_config_mac_port1_loopback();
#else
#define INIT_PORT1_PHY star_gsw_config_VSC8201(1,1);
#endif
//#define INIT_PORT1_PHY 

// configure mac0/mac1 register
#define INIT_PORT0_MAC init_packet_forward(0);
#define INIT_PORT1_MAC init_packet_forward(1);
//#define INIT_PORT1_MAC 

#define PORT0_LINK_DOWN disable_AN(0, 0);
#define PORT0_LINK_UP disable_AN(0, 1);

#ifdef DORADO2_PCI_FASTPATH_MAC_PORT1_LOOPBACK
#define PORT1_LINK_DOWN
#define PORT1_LINK_UP
#else
#define PORT1_LINK_DOWN std_phy_power_down(1, 1);
#define PORT1_LINK_UP std_phy_power_down(1, 0);
#endif

#define CONFIG_STR9100_PORT_BASE
#define CONFIG_STR9100_VLAN_BASE
#define CONFIG_HAVE_VLAN_TAG


#define MODEL "DORADO2"

static int rc_port0 = 0; // rc mean reference counting, determine port open/close.
//static int rc_port1 = 0; // rc mean reference counting, determine port open/close.


// enable port
// link down
static inline void open_port0(void)
{
        if (rc_port0 == 0) {
		enable_port(0);
		PRINT_INFO("open mac port 0\n");
		// link up
		disable_AN(0, 1);
	} else {
		PRINT_INFO("port 0 already open\n");\
	}
	++rc_port0;
}

static inline void close_port0(void)
{
	--rc_port0;
        if (rc_port0 == 0) {
		// link down
		disable_AN(0, 0);
		disable_port(0);
       		PRINT_INFO("close mac port 0\n");\
	}
}

static inline void open_port1()
{

	enable_port(1);
	PRINT_INFO("open mac port 1\n");
	// link up
	std_phy_power_down(1, 0);
}

static inline void close_port1()
{
	disable_port(1);
	PRINT_INFO("close mac port 1\n");
	// link down
	std_phy_power_down(1, 1);
}

#define VLAN0_VID			(0x2)
#define VLAN1_VID			(0x1)
#define VLAN2_VID			(0x3)
#define VLAN3_VID			(0x4)
#define VLAN4_VID			(0x5)
#define VLAN5_VID			(0x6)
#define VLAN6_VID			(0x7)
#define VLAN7_VID			(0x8)

#define VLAN0_GROUP			(PORT0 | CPU_PORT)
#define VLAN1_GROUP			(PORT0 | CPU_PORT)
#define VLAN2_GROUP			(PORT1 | CPU_PORT)
#define VLAN3_GROUP			(PORT1 | CPU_PORT)
#define VLAN4_GROUP			(0)
#define VLAN5_GROUP			(0)
#define VLAN6_GROUP			(0)
#define VLAN7_GROUP			(PORT1 | CPU_PORT)


#ifdef CONFIG_HAVE_VLAN_TAG

#define VLAN0_VLAN_TAG			(5)	// cpu port and mac 0 port
#define VLAN1_VLAN_TAG			(5)	// cpu port and mac 0 port

#else
#define VLAN0_VLAN_TAG			(1)	// only mac 0 port
#define VLAN1_VLAN_TAG			(1)	// only mac 0 port
#endif

#define VLAN2_VLAN_TAG			(0)
#define VLAN3_VLAN_TAG			(0)
#define VLAN4_VLAN_TAG			(0)
#define VLAN5_VLAN_TAG			(0)
#define VLAN6_VLAN_TAG			(0)
#define VLAN7_VLAN_TAG			(0)


/* wan eth1 */
static u8 my_vlan0_mac[6] = {0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0x50};

/* lan eth 0*/
static u8 my_vlan1_mac[6] = {0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0x60};

/* cpu */
static u8 my_vlan2_mac[6] = {0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0x22};

/* ewc  */
static u8 my_vlan3_mac[6] = {0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0x23};


#endif //DORADO2

static NetDevicePriv net_device_prive[]= {
				   /*  pmap, is_wan, gid, vlan_tag, rx_func_ptr, tx_func_ptr, open_ptr, close_ptr, netdev name */
					   {1, 0, 1, 1, rx_vlan_base, tx_vlan_base, open_port0, close_port0, 0},   // eth0 LAN
					   {1, 0, 0, 2, rx_vlan_base, tx_vlan_base, open_port0, close_port0, 0},   // eth1 WAN 
					   {2, 0, 0, PORT1_NETDEV_INDEX, rx_port_base, tx_port_base, open_port1, close_port1, 0}   // eth2 LAN

					 };



#endif
