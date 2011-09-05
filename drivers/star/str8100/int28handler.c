/*======================================================================

   Copyright (C) 2006 STAR Semiconductor Limited

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

======================================================================*/
 
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>

#include <asm/arch/star_gpio.h>
#include <asm/arch/star_intc.h>
#include <asm/arch/star_misc.h>
#include <asm/arch/star_powermgt.h>

extern void str8100_set_interrupt_trigger(unsigned int, unsigned int, unsigned int);
static struct work_struct reboot_work;
void do_reboot(){
	char* argv[2];
	char* env[1];
	argv[0]="/sbin/reboot";
	argv[1]=NULL;
	env[0]=NULL;
	call_usermodehelper(argv[0],argv,env,0);

}
static irqreturn_t str8100_int28vbus_irq_handler(int this_irq, void *dev_id, struct pt_regs *regs)
{
	printk("%s: this_irq=%d\n",__FUNCTION__,this_irq);

printk("%s: registering work\n",__FUNCTION__);
	INIT_WORK(&reboot_work,do_reboot,NULL);
	if(PWRMGT_USB_DEVICE_POWERMGT_REG&0x1)
		schedule_work(&reboot_work);
	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(this_irq);
	return IRQ_HANDLED;
}

static int __init str8100_int28vbus_inthandler_init(void)
{
	int ret;
printk("%s: \n",__FUNCTION__);
	if(PWRMGT_USB_DEVICE_POWERMGT_REG&0x1)
		HAL_PWRMGT_GLOBAL_SOFTWARE_RESET();
	//	schedule_work(&reboot_work);

	//str8100_set_interrupt_trigger (INTC_GPIO_EXTERNAL_INT_BIT_INDEX,INTC_LEVEL_TRIGGER,INTC_ACTIVE_HIGH);
	if ((ret=request_irq(INTC_USB_DEVICE_VBUS_BIT_INDEX, str8100_int28vbus_irq_handler, 0, "vbus", NULL))){
		printk("%s: request_irq failed(ret=0x%x)(-EBUSY=0x%x)\n",__FUNCTION__,ret,-EBUSY);
		return -EBUSY;
	}
	return 0;
}

static void __exit str8100_int28vbus_inthandler_exit(void) 
{ 
	printk("%s: \n",__FUNCTION__);
	lock_kernel();
	free_irq(INTC_USB_DEVICE_VBUS_BIT_INDEX, NULL);
	unlock_kernel();
}

module_init(str8100_int28vbus_inthandler_init);
module_exit(str8100_int28vbus_inthandler_exit);

