/*******************************************************************************
 *
 *
 *   Copyright (c) 2008 Cavium Networks 
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

//#include <linux/module.h>
#include <linux/types.h>
#include <linux/ethtool.h>
#include <linux/netdevice.h>

// ethtool support reference e100.c and e1000_ethtool.c .
static void str9100_get_drvinfo(struct net_device *netdev, struct ethtool_drvinfo *info)
{
	strcpy(info->driver, "str9100 test");
}

extern int RX_DESC_SIZE;
extern int TX_DESC_SIZE;

static void str9100_get_ringparam(struct net_device *netdev,
        struct ethtool_ringparam *ring)
{

        ring->rx_max_pending = RX_DESC_SIZE;
        ring->tx_max_pending = TX_DESC_SIZE;
#if 0
        struct nic *nic = netdev_priv(netdev);
        struct param_range *rfds = &nic->params.rfds;
        struct param_range *cbs = &nic->params.cbs;

        ring->rx_max_pending = rfds->max;
        ring->tx_max_pending = cbs->max;
        ring->rx_mini_max_pending = 0;
        ring->rx_jumbo_max_pending = 0;
        ring->rx_pending = rfds->count;
        ring->tx_pending = cbs->count;
        ring->rx_mini_pending = 0;
        ring->rx_jumbo_pending = 0;
#endif
}

static int str9100_set_ringparam(struct net_device *netdev,
        struct ethtool_ringparam *ring)
{
#if 0
        struct nic *nic = netdev_priv(netdev);
        struct param_range *rfds = &nic->params.rfds;
        struct param_range *cbs = &nic->params.cbs;

        if ((ring->rx_mini_pending) || (ring->rx_jumbo_pending))
                return -EINVAL;

        if(netif_running(netdev))
                e100_down(nic);
        rfds->count = max(ring->rx_pending, rfds->min);
        rfds->count = min(rfds->count, rfds->max);
        cbs->count = max(ring->tx_pending, cbs->min);
        cbs->count = min(cbs->count, cbs->max);
        DPRINTK(DRV, INFO, "Ring Param settings: rx: %d, tx %d\n",
                rfds->count, cbs->count);
        if(netif_running(netdev))
                e100_up(nic);

#endif
        //ring->rx_max_pending = RX_DESC_SIZE;
        //ring->tx_max_pending = TX_DESC_SIZE;
        return 0;
}

static uint32_t str9100_get_tx_csum(struct net_device *netdev)
{
        //return (netdev->features & NETIF_F_HW_CSUM) != 0;
        return (netdev->features & NETIF_F_IP_CSUM) != 0;
}

static int str9100_set_tx_csum(struct net_device *netdev, uint32_t data)
{
       if (data)
                netdev->features |= NETIF_F_IP_CSUM;
        else
                netdev->features &= ~NETIF_F_IP_CSUM;
	return 0;
}

static uint32_t str9100_get_rx_csum(struct net_device *netdev)
{
        //struct e1000_adapter *adapter = netdev_priv(netdev);
        //return adapter->rx_csum;
}

static int str9100_set_rx_csum(struct net_device *netdev, uint32_t data)
{
}

u32 str9100_get_sg(struct net_device *dev)
{
#ifdef NETIF_F_SG
        return (dev->features & NETIF_F_SG) != 0;
#else
        return 0;
#endif
}

int str9100_set_sg(struct net_device *dev, u32 data)
{
#ifdef NETIF_F_SG 
        if (data)
                dev->features |= NETIF_F_SG;
        else
                dev->features &= ~NETIF_F_SG;
#endif

        return 0;
}





static const struct ethtool_ops str9100_ethtool_ops = {
	.get_drvinfo            = str9100_get_drvinfo,
        .get_ringparam          = str9100_get_ringparam,
        .set_ringparam          = str9100_set_ringparam,
        .get_rx_csum            = str9100_get_rx_csum,
        .set_rx_csum            = str9100_set_rx_csum,
        .get_tx_csum            = str9100_get_tx_csum,
        .set_tx_csum            = str9100_set_tx_csum,
        .get_sg  	        = str9100_get_sg,
        .set_sg                 = str9100_set_sg,
};

void str9100_set_ethtool_ops(struct net_device *netdev)
{
	SET_ETHTOOL_OPS(netdev, &str9100_ethtool_ops);
}
