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

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <linux/interrupt.h>

#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/hardware.h>
#include <asm/mach-types.h>

#include <asm/arch/star_powermgt.h>
#include <asm/arch/star_intc.h>
#include <asm/arch/star_misc.h>
#include <asm/arch/star_gpio.h>

#ifdef CONFIG_STR8100_GPIO_INTERRUPT

#define MAX_GPIOA_LINE		32
#define MAX_GPIOB_LINE		32
#define PIN_INPUT 0
#define PIN_OUTPUT 1

#define PIN_TRIG_EDGE 0
#define PIN_TRIG_LEVEL 1

#define PIN_TRIG_SINGLE 0
#define PIN_TRIG_BOTH 1

#define PIN_TRIG_RISING 0
#define PIN_TRIG_FALLING 1

#define PIN_TRIG_HIGH 0
#define PIN_TRIG_LOW 1
void (*gpio_a_isr[MAX_GPIOA_LINE])(int i);
void (*gpio_b_isr[MAX_GPIOB_LINE])(int i);

#endif

#if 0
int __init_or_module gpio_direction_input(unsigned int);
int __init_or_module gpio_direction_output(unsigned int, unsigned int);
void gpio_set_value(unsigned int, unsigned int);
int gpio_get_value(unsigned int);
void str8100_gpio_a_set_edgeintr(void (*funcptr)(int), int, int, int);
void str8100_gpio_b_set_edgeintr(void (*funcptr)(int),int,int, int);
void str8100_gpio_a_set_levelintr(void (*funcptr)(int),int, int);
void str8100_gpio_b_set_levelintr(void (*funcptr)(int),int, int);
#endif

/* 
 * str8100_gpio_a_in - read Data Input Register(RO) of GPIO A
 * @data: the target to store content of register
 */
int str8100_gpio_a_datain(volatile __u32 *data)
{
	HAL_GPIOA_READ_DATA_IN_STATUS(*data);
	return 0;
}

/* 
 * str8100_gpio_b_in - read Data Input Register(RO) of GPIO B
 * @data: the target to store content of register
 */
int str8100_gpio_b_datain(volatile __u32 *data)
{
	HAL_GPIOB_READ_DATA_IN_STATUS(*data);
	return 0;
}

/* str8100_gpio_a_out - write Data Output Register(RW) of GPIO A
 * @data:
 */
int str8100_gpio_a_dataout(__u32 data)
{
	GPIOA_DATA_OUTPUT_REG = data;
	return 0;
}

/* 
 * str8100_gpio_b_out - write Data Output Register(RW) of GPIO B
 * @data:
 */
int str8100_gpio_b_out(__u32 data)
{
	GPIOB_DATA_OUTPUT_REG = data;
	return 0;
}

/* 
 * str8100_gpio_a_read_direction - read Direction Register(RW) GPIO A
 * @data:
 */
int str8100_gpio_a_read_direction(volatile __u32 *data)
{
	*data = GPIOA_DIRECTION_REG;
	return 0;
}

/* 
 * str8100_gpio_b_read_direction - read Direction Register(RW) of GPIO B
 * @data:
 */
int str8100_gpio_b_read_direction(volatile __u32 *data)
{
	*data = GPIOB_DIRECTION_REG;
	return 0;
}

/* 
 * str8100_gpio_a_write_direction - write Direction Register(RW) of GPIO A
 * @data:
 */
int str8100_gpio_a_write_direction(__u32 data)
{
	GPIOA_DIRECTION_REG = data;
	return 0;
}

/* 
 * str8100_gpio_b_write_direction - write Direction Register(RW) of GPIO B
 * @data:
 */
int str8100_gpio_b_write_direction(__u32 data)
{
	GPIOB_DIRECTION_REG = data;
	return 0;
}

/* 
 * str8100_gpio_a_dataset - write Data Bit Set Register (W) of GPIO A
 * @data:
 *
 * When write to this register and if some bits of GpioDataSet are 1,
 * the corresponding bits in GpioDataOut register will be set to 1, 
 * and the others will not be changed.
 */
int str8100_gpio_a_dataset(__u32 data)
{
	GPIOA_DATA_BIT_SET_REG = data;
	return 0;
}

/* 
 * str8100_gpio_b_dataset - write Data Bit Set Register (W) of GPIO B
 * @data:
 */
int str8100_gpio_b_dataset(__u32 data)
{
	GPIOB_DATA_BIT_SET_REG = data;
	return 0;
}

/* 
 * str8100_gpio_a_dataclear - write Data Bit Clear Register (W) of GPIO A
 * @data:
 *
 * When write to this register and if some bits of GpioDataClear are 1,
 * the corresponding bits in GpioDataOut register will be cleard, 
 * and the others will not be changed.
 */
int str8100_gpio_a_dataclear(__u32 data)
{
	GPIOA_DATA_BIT_CLEAR_REG = data;
	return 0;
}

/* 
 * str8100_gpio_b_dataclear - write Data Bit Clear Register (W) of GPIO B
 * @data:
 */
int str8100_gpio_b_dataclear(__u32 data)
{
	GPIOB_DATA_BIT_CLEAR_REG = data;
	return 0;
}
/*
Read String into Buffer, Max String buffer is 100
*/
ssize_t readstring(char *buff, const char *buf, size_t count){
    	int i=0;
        if (count) {
                char c;

            for(i=0;i<count&&i<100;i++){
                if (get_user(c, buf+i))
                        return -EFAULT;
                    buff[i] = c;
            }
                buff[i]=0;
        }
        return count;

}

#ifdef CONFIG_STR8100_GPIO_GENERIC_INTERFACE
static int str8100_gpio_proc(char *page, char **start,  off_t off, int count, int *eof, void *data)
{
	int num = 0;

	num += sprintf(page+num, "********** GPIO Group A **********\n");
	
	num += sprintf(page+num, "GPIO IN                : %08x \n", GPIOA_DATA_INPUT_REG);

	num += sprintf(page+num, "GPIO Direction         : %08x \n", GPIOA_DIRECTION_REG);

#ifdef CONFIG_STR8100_GPIO_INTERRUPT
	num += sprintf(page+num, "GPIO Interrupt Enable  : %08x \n", GPIOA_INTERRUPT_ENABLE_REG);

	num += sprintf(page+num, "GPIO Interrupt Raw     : %08x \n", GPIOA_INTERRUPT_RAW_STATUS_REG);

	num += sprintf(page+num, "GPIO Interrupt Trigger : %08x \n", GPIOA_INTERRUPT_TRIGGER_METHOD_REG);

	num += sprintf(page+num, "GPIO Interrupt Both    : %08x \n", GPIOA_INTERRUPT_TRIGGER_BOTH_EDGES_REG);

	num += sprintf(page+num, "GPIO Interrupt RiseNeg : %08x \n", GPIOA_INTERRUPT_TRIGGER_TYPE_REG);

	num += sprintf(page+num, "GPIO Interrupt MASKED  : %08x \n", GPIOA_INTERRUPT_MASK_REG);

	num += sprintf(page+num, "GPIO Interrupt MASKEDST: %08x \n", GPIOA_INTERRUPT_MASKED_STATUS_REG);
#endif	

	num+= sprintf(page+num, "********** GPIO Group B **********\n");
	
	num += sprintf(page+num, "GPIO IN                : %08x \n", GPIOB_DATA_INPUT_REG);

	num += sprintf(page+num, "GPIO Direction         : %08x \n", GPIOB_DIRECTION_REG);

#ifdef CONFIG_STR8100_GPIO_INTERRUPT
	num += sprintf(page+num, "GPIO Interrupt Enable  : %08x \n", GPIOB_INTERRUPT_ENABLE_REG);

	num += sprintf(page+num, "GPIO Interrupt Raw     : %08x \n", GPIOB_INTERRUPT_RAW_STATUS_REG);

	num += sprintf(page+num, "GPIO Interrupt Trigger : %08x \n", GPIOB_INTERRUPT_TRIGGER_METHOD_REG);

	num += sprintf(page+num, "GPIO Interrupt Both    : %08x \n", GPIOB_INTERRUPT_TRIGGER_BOTH_EDGES_REG);

	num += sprintf(page+num, "GPIO Interrupt RiseNeg : %08x \n", GPIOB_INTERRUPT_TRIGGER_TYPE_REG);

	num += sprintf(page+num, "GPIO Interrupt MASKED  : %08x \n", GPIOB_INTERRUPT_MASK_REG);

	num += sprintf(page+num, "GPIO Interrupt MASKEDST: %08x \n", GPIOB_INTERRUPT_MASKED_STATUS_REG);
#endif	

	return num;
}
#endif

#ifdef CONFIG_STR8100_GPIO_INTERRUPT
static irqreturn_t str8100_gpio_irq_handler(int this_irq, void *dev_id, struct pt_regs *regs)
{
	unsigned int volatile    status;
	int i;

	// Clean System irq status
	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(INTC_GPIO_EXTERNAL_INT_BIT_INDEX);
	HAL_INTC_DISABLE_INTERRUPT_SOURCE(INTC_GPIO_EXTERNAL_INT_BIT_INDEX);

	HAL_GPIOA_READ_INTERRUPT_MASKED_STATUS(status);
	for (i = 0; i < MAX_GPIOA_LINE; i++) {   
		if (status & (1 << i))  {   		/* interrupt is detected and not masked */
			if (gpio_a_isr[i] != NULL) {
				gpio_a_isr[i](i);
			}
		}    
	}  
    HAL_GPIOA_CLEAR_INTERRUPT(status);

	HAL_GPIOB_READ_INTERRUPT_MASKED_STATUS(status);
	for (i = 0; i < MAX_GPIOB_LINE; i++) {   
		if (status & (1 << i)) {			/* interrupt is detected and not masked */
			if (gpio_b_isr[i] != NULL) {
				gpio_b_isr[i](i);
			}
		}    
	}   
    HAL_GPIOB_CLEAR_INTERRUPT(status);

	/* Unmask Intc Interrupt Status */
	HAL_INTC_ENABLE_INTERRUPT_SOURCE(INTC_GPIO_EXTERNAL_INT_BIT_INDEX);
	return IRQ_HANDLED;
}

int intr_a_count=0;
int intr_b_count=0;
/*  
 * Setup GPIOA for Edge Trigger Interrupt mode 
 */
void str8100_gpio_a_set_edgeintr(void (*funcptr)(int), int trig_both, int trig_rising, int gpio_pin)
{
	u32 gpio_index = (0x1 << gpio_pin);
	
	if (gpio_pin >= 0 && gpio_pin < MAX_GPIOA_LINE) {
		HAL_GPIOA_SET_DIRECTION_INPUT(gpio_index);

		if (trig_both == PIN_TRIG_BOTH) {
			/* Set Trigger Both */
			HAL_GPIOA_SET_INTERRUPT_BOTH_EDGE_TRIGGER_MODE(gpio_index);	
		}
		else if (trig_both == PIN_TRIG_SINGLE) {
			/* Set Single Rising/Falling Edge Trigger */
			if (trig_rising == PIN_TRIG_RISING) {
				HAL_GPIOA_SET_INTERRUPT_SINGLE_RISING_EDGE_TRIGGER_MODE(gpio_index);
			}
			else if (trig_rising == PIN_TRIG_FALLING) {
				HAL_GPIOA_SET_INTERRUPT_SINGLE_FALLING_EDGE_TRIGGER_MODE(gpio_index);
			}
			else {
				printk("Trigger rising/falling error.\n");
				return;
			}
		}
		else
		{
			printk("Trigger both/single error.\n");
			return;
		}

		gpio_a_isr[gpio_pin] = funcptr;

		/* Enable Interrupt */
		HAL_GPIOA_ENABLE_INTERRUPT(gpio_index);
	}
}

/*  
 * Setup GPIOB for Edge Trigger Interrupt mode 
 */
void str8100_gpio_b_set_edgeintr(void (*funcptr)(int),int trig_both,int trig_rising, int gpio_pin)
{
	u32 gpio_index = (0x1 << gpio_pin);
	
	if (gpio_pin >= 0 && gpio_pin < MAX_GPIOA_LINE) {
		HAL_GPIOB_SET_DIRECTION_INPUT(gpio_index);

		if (trig_both == PIN_TRIG_BOTH) {
			/* Set Trigger Both */
			HAL_GPIOB_SET_INTERRUPT_BOTH_EDGE_TRIGGER_MODE(gpio_index);	
		}
		else if (trig_both == PIN_TRIG_SINGLE) {
			/* Set Single Rising/Falling Edge Trigger */
			if (trig_rising == PIN_TRIG_RISING) {
				HAL_GPIOB_SET_INTERRUPT_SINGLE_RISING_EDGE_TRIGGER_MODE(gpio_index);
			}
			else if (trig_rising == PIN_TRIG_FALLING) {
				HAL_GPIOB_SET_INTERRUPT_SINGLE_FALLING_EDGE_TRIGGER_MODE(gpio_index);
			}
			else {
				printk("Trigger rising/falling error.\n");
				return;
			}
		}
		else {
			printk("Trigger both/single error.\n");
			return;
		}

		gpio_b_isr[gpio_pin] = funcptr;

		/* Enable Interrupt */
		HAL_GPIOB_ENABLE_INTERRUPT(gpio_index);
	}
}

/*  
 * Clear GPIOA Trigger Interrupt
 */
void str8100_gpio_a_clear_intr(int gpio_pin)
{
	if (gpio_pin >= 0 && gpio_pin < MAX_GPIOA_LINE) {
		/* Unregister isr of GPIOA[gpio_pin] */	
		gpio_a_isr[gpio_pin] = NULL;	
		/* Disable Interrupt */
		HAL_GPIOA_DISABLE_INTERRUPT(0x1 << gpio_pin);
	}
}

/*  
 * Clear GPIOB Trigger Interrupt
 */
void str8100_gpio_b_clear_intr(int gpio_pin)
{
	if (gpio_pin >= 0 && gpio_pin < MAX_GPIOB_LINE) {
		/* Unregister isr of GPIOB[gpio_pin] */
		gpio_b_isr[gpio_pin] = NULL;			
		/* Disable Interrupt */
		HAL_GPIOB_DISABLE_INTERRUPT(0x1 << gpio_pin);
	}
}

/*  
 * Setup GPIOA for LEVEL Trigger Interrupt mode 
 */
void str8100_gpio_a_set_levelintr(void (*funcptr)(int),int trig_level, int gpio_pin)
{
	u32 gpio_index = (0x1 << gpio_pin);

	if (gpio_pin >= 0 && gpio_pin < MAX_GPIOA_LINE) {
		HAL_GPIOA_SET_DIRECTION_INPUT(gpio_index);

		/* Set Trigger High/Low */
		if (trig_level == PIN_TRIG_HIGH) {
			HAL_GPIOA_SET_INTERRUPT_HIGH_LEVEL_TRIGGER_MODE(gpio_index);
		}
		else if (trig_level == PIN_TRIG_LOW) {
			HAL_GPIOA_SET_INTERRUPT_LOW_LEVEL_TRIGGER_MODE(gpio_index);
		}
		else {
			printk("Trigger level error.\n");
			return;
		}

		gpio_a_isr[gpio_pin] = funcptr;

		/* Enable Interrupt */
		HAL_GPIOA_ENABLE_INTERRUPT(gpio_index);
	}
}

/*  
 * Setup GPIO for LEVEL Triggle Interrupt mode 
 */
void str8100_gpio_b_set_levelintr(void (*funcptr)(int),int trig_level, int gpio_pin)
{
	u32 gpio_index = (0x1 << gpio_pin);

	if (gpio_pin >= 0 && gpio_pin < MAX_GPIOB_LINE) {
		HAL_GPIOB_SET_DIRECTION_INPUT(gpio_index);

		/* Set Trigger High/Low */
		if (trig_level == PIN_TRIG_HIGH) {
			HAL_GPIOB_SET_INTERRUPT_HIGH_LEVEL_TRIGGER_MODE(gpio_index);
		}
		else if (trig_level == PIN_TRIG_LOW) {
			HAL_GPIOB_SET_INTERRUPT_LOW_LEVEL_TRIGGER_MODE(gpio_index);
		}
		else {
			printk("Trigger level error.\n");
			return;
		}

		gpio_b_isr[gpio_pin] = funcptr;

		/* Enable Interrupt */
		HAL_GPIOB_ENABLE_INTERRUPT(gpio_index);
	}
}

EXPORT_SYMBOL(str8100_gpio_a_set_edgeintr);
EXPORT_SYMBOL(str8100_gpio_a_clear_intr);
EXPORT_SYMBOL(str8100_gpio_a_set_levelintr);
EXPORT_SYMBOL(str8100_gpio_b_set_edgeintr);
EXPORT_SYMBOL(str8100_gpio_b_clear_intr);
EXPORT_SYMBOL(str8100_gpio_b_set_levelintr);

/*  
 * Display GPIO information at /proc/str9100/gpio
 */

#ifdef STR8100_GPIO_INTERRUPT_TEST
void str8100_gpio_intr_test(int i)
{
	printk("GPIO Interrupt Service Single Active : %d \n",i);
}
#endif

#endif

#ifdef CONFIG_STR8100_GPIO_GENERIC_INTERFACE
/***********************************************************************
 * The STR8100 has GPIOA(32) and GPIOB(32) total 64 GPIO. For the
 * generic GPIO interface, the GPIO pin number count from GPIOA to GPIOB.
 * For example:
 *      0 -> GPIOA[0]
 *      1 -> GPIOA[1]
 *     ......
 *     31 -> GPIOA[31]
 *     32 -> GPIOB[0]
 *     33 -> GPIOB[1]
 *     ......
 *     63 -> GPIOB[31]
 **********************************************************************/

#define GPIOA_PIN_NO	32
#define GPIOB_PIN_NO	32
#define MAX_GPIO_NO	(GPIOA_PIN_NO + GPIOB_PIN_NO)

/*
 * Configure the GPIO line as an input.
 */
int __init_or_module gpio_direction_input(unsigned int pin)
{
	volatile __u32 reg;
	unsigned long flags;

	if (pin >= MAX_GPIO_NO)
		return -EINVAL;

        local_irq_save(flags);

	/* Clear register bit to set as input pin. */
	if (pin < GPIOA_PIN_NO)
	{
		/* GPIOA */
		reg = GPIOA_DIRECTION_REG;
		reg &= ~(1 << pin);
		GPIOA_DIRECTION_REG = reg;
	}
	else
	{
		/* GPIOB */
		reg = GPIOB_DIRECTION_REG;
		reg &= ~(1 << (pin - GPIOA_PIN_NO));
		GPIOB_DIRECTION_REG = reg;
	}

        local_irq_restore(flags);

        return 0;
}
//EXPORT_SYMBOL(gpio_direction_input);

/*
 * Configure the GPIO line as an output, with default state.
 */
int __init_or_module gpio_direction_output(unsigned int pin, unsigned int state)
{
	volatile __u32 reg;
	unsigned long flags;
	if (pin >= MAX_GPIO_NO)
		return -EINVAL;

        local_irq_save(flags);

	if (pin < GPIOA_PIN_NO)
	{
		/* GPIOA */
		/* Set register bit to set as output pin. */
		reg = GPIOA_DIRECTION_REG;
		reg |= (1 << pin);
		GPIOA_DIRECTION_REG = reg;

		if (state)
			GPIOA_DATA_BIT_SET_REG = (1 << pin);
		else
			GPIOA_DATA_BIT_CLEAR_REG = (1 << pin);

	}
	else
	{
		/* GPIOB */
		/* Set register bit to set as output pin. */
		reg = GPIOB_DIRECTION_REG;
		reg |= (1 << (pin - GPIOA_PIN_NO));
		GPIOB_DIRECTION_REG = reg;

		if (state)
			GPIOB_DATA_BIT_SET_REG = (1 << (pin - GPIOA_PIN_NO));
		else
			GPIOB_DATA_BIT_CLEAR_REG = (1 << (pin - GPIOA_PIN_NO));
	}

        local_irq_restore(flags);

        return 0;
}
EXPORT_SYMBOL(gpio_direction_output);


/*
 * Set the state of an output GPIO line.
 */
void gpio_set_value(unsigned int pin, unsigned int state)
{
	if (pin >= MAX_GPIO_NO)
		return;

	if (pin < GPIOA_PIN_NO)
	{
		/* GPIOA */
		if (state)
			GPIOA_DATA_BIT_SET_REG = (1 << pin);
		else
			GPIOA_DATA_BIT_CLEAR_REG = (1 << pin);

	}
	else
	{
		/* GPIOB */
		if (state)
			GPIOB_DATA_BIT_SET_REG = (1 << (pin - GPIOA_PIN_NO));
		else
			GPIOB_DATA_BIT_CLEAR_REG = (1 << (pin - GPIOA_PIN_NO));
	}
}
EXPORT_SYMBOL(gpio_set_value);


/*
 * Read the state of a GPIO line.
 */
int gpio_get_value(unsigned int pin)
{
	volatile __u32 reg;
	bool bret = 0;

	if (pin >= MAX_GPIO_NO)
		return -EINVAL;

	if (pin < GPIOA_PIN_NO)
	{
		/* GPIOA */
		str8100_gpio_a_datain(&reg);
		bret = (reg & (1 << pin)) != 0;
	}
	else
	{
		/* GPIOB */
		str8100_gpio_b_datain(&reg);
		bret = (reg & (1 << (pin - GPIOA_PIN_NO))) != 0;
	}
	
	return bret;
}
EXPORT_SYMBOL(gpio_get_value);


/*
 * Map GPIO line to IRQ number.
 */
int gpio_to_irq(unsigned int pin)
{
	return INTC_GPIO_EXTERNAL_INT_BIT_INDEX;
}
EXPORT_SYMBOL(gpio_to_irq);


/*
 * INVALID
 */
int irq_to_gpio(unsigned int irq)
{
	return -EINVAL;
}
EXPORT_SYMBOL(irq_to_gpio);

#endif /* CONFIG_STR8100_GPIO_GENREIC_INTERFACE */

#ifdef CONFIG_STR8100_GPIO_INTERRUPT
static void gpio_a_isr_test(int i)
{
	unsigned int volatile status = 0;

	HAL_INTC_DISABLE_INTERRUPT_SOURCE(INTC_GPIO_EXTERNAL_INT_BIT_INDEX);
	printk("gpio_a_isr_test, count:%d\n",intr_a_count+1);
	status = GPIOA_INTERRUPT_MASKED_STATUS_REG;
	HAL_GPIOA_CLEAR_INTERRUPT(status);
	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(INTC_GPIO_EXTERNAL_INT_BIT_INDEX);

	intr_a_count++;
	if (intr_a_count >= 4) {
		str8100_gpio_a_clear_intr(i);
		intr_a_count = 0;
	}

	HAL_INTC_ENABLE_INTERRUPT_SOURCE(INTC_GPIO_EXTERNAL_INT_BIT_INDEX);
}

static void gpio_b_isr_test(int i)
{
	unsigned int volatile status = 0;

	HAL_INTC_DISABLE_INTERRUPT_SOURCE(INTC_GPIO_EXTERNAL_INT_BIT_INDEX);
	printk("gpio_b_isr_test, count:%d\n",intr_b_count+1);
	status = GPIOB_INTERRUPT_MASKED_STATUS_REG;
	HAL_GPIOB_CLEAR_INTERRUPT(status);
	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(INTC_GPIO_EXTERNAL_INT_BIT_INDEX);

	intr_b_count++;
	if (intr_b_count >= 4) {
		str8100_gpio_b_clear_intr(i);
		intr_b_count = 0;
	}

	HAL_INTC_ENABLE_INTERRUPT_SOURCE(INTC_GPIO_EXTERNAL_INT_BIT_INDEX);
}
#endif

#ifdef CONFIG_STR8100_GPIO_GENERIC_INTERFACE
static ssize_t str8100_gpio_write_proc(struct file *file, const char __user *buffer,
			   ssize_t count, void *data){
	int pin = 0,state =0;
	char read_buff[100],buf_cmd[100],buf_param1[100],buf_param2[100],buf_param3[100],buf_param4[100];
	readstring((char *)read_buff,(const char *)buffer,count);
	sscanf(read_buff,"%s %s %s %s %s\n",(char *)&buf_cmd,(char *)&buf_param1\
									   ,(char *)&buf_param2, (char *)&buf_param3, (char *)&buf_param4);
	//printk("buf_cmd:%s buf_param1:%s buf_param2:%s buf_param2:%s \n", buf_cmd,buf_param1,buf_param2,buf_param3);
	if(strcmp(buf_cmd,"direct") == 0)
		if(strcmp(buf_param1,"input") == 0){
			sscanf(buf_param2, "%d", &pin);
			//printk("direction input pin=%d \n",pin);
			gpio_direction_input(pin);
		}
	if(strcmp(buf_cmd,"direct") == 0)
		if(strcmp(buf_param1,"output") == 0){
			sscanf(buf_param2, "%d", &pin);
			sscanf(buf_param3, "%d", &state);
			//printk("direction output pin=%d state=%d  \n",pin,state);
			gpio_direction_output(pin,state);
		}
	if(strcmp(buf_cmd,"set") == 0)
		if(strcmp(buf_param1,"value") == 0){
			sscanf(buf_param2, "%d", &pin);
			sscanf(buf_param3, "%d", &state);
			printk("set value pin=%d state=%d  \n",pin,state);
			gpio_set_value(pin,state);
		}
	if(strcmp(buf_cmd,"get") == 0)
		if(strcmp(buf_param1,"value") == 0){
			sscanf(buf_param2, "%d", &pin);
			state=gpio_get_value(pin);
			printk("get value pin=%d state=%d \n",pin,state);
		}

#ifdef CONFIG_STR8100_GPIO_INTERRUPT
	if(strcmp(buf_cmd,"trig") == 0){
		if(strcmp(buf_param1,"edge") == 0){
			if(strcmp(buf_param2,"both") == 0){
				sscanf(buf_param3, "%d", &pin);
				if (pin < GPIOA_PIN_NO)
					str8100_gpio_a_set_edgeintr(&gpio_a_isr_test, 1, 0, pin);
				else
					str8100_gpio_b_set_edgeintr(&gpio_b_isr_test, 1, 0, pin-GPIOA_PIN_NO);		
			}
			if(strcmp(buf_param2,"single") == 0){
				if(strcmp(buf_param3,"rising") == 0){
					sscanf(buf_param4, "%d", &pin);
					if (pin < GPIOA_PIN_NO)
						str8100_gpio_a_set_edgeintr(&gpio_a_isr_test, 0, 0, pin);
					else
						str8100_gpio_b_set_edgeintr(&gpio_b_isr_test, 0, 0, pin-GPIOA_PIN_NO);
				}
				if(strcmp(buf_param3,"falling") == 0){
					sscanf(buf_param4, "%d", &pin);
					if (pin < GPIOA_PIN_NO)
						str8100_gpio_a_set_edgeintr(&gpio_a_isr_test, 0, 1, pin);
					else
						str8100_gpio_b_set_edgeintr(&gpio_b_isr_test, 0, 1, pin-GPIOA_PIN_NO);
				}		
			}
		}
		if(strcmp(buf_param1,"level") == 0){
			if(strcmp(buf_param2,"high") == 0){
				sscanf(buf_param3, "%d", &pin);
				if (pin < GPIOA_PIN_NO)
					str8100_gpio_a_set_levelintr(&gpio_a_isr_test, 0, pin);
				else
					str8100_gpio_b_set_levelintr(&gpio_b_isr_test, 0, pin-GPIOA_PIN_NO);
			}
			if(strcmp(buf_param2,"low") == 0){
				sscanf(buf_param3, "%d", &pin);
				if (pin < GPIOA_PIN_NO)
					str8100_gpio_a_set_levelintr(&gpio_a_isr_test, 1, pin);
				else
					str8100_gpio_b_set_levelintr(&gpio_b_isr_test, 1, pin-GPIOA_PIN_NO);
			}
		}
	}
#endif
		
	return count;
	
}

static struct proc_dir_entry *proc_str8100_gpio;
#endif
extern void str8100_set_interrupt_trigger(unsigned int, unsigned int, unsigned int);

int __init str8100_gpio_init(void)
{
#ifdef CONFIG_STR8100_GPIO_INTERRUPT
	u32 i, ret;
#endif

	//proc_str8100_gpio = create_proc_read_entry("str8100/gpio", 0, NULL, str8100_gpio_proc, NULL) ;
#ifdef CONFIG_STR8100_GPIO_GENERIC_INTERFACE
	proc_str8100_gpio = create_proc_entry("str8100/gpio", S_IFREG | S_IRUGO, NULL) ;
	proc_str8100_gpio->read_proc = str8100_gpio_proc;
	proc_str8100_gpio->write_proc = str8100_gpio_write_proc;
#endif

#ifdef CONFIG_STR8100_GPIO_INTERRUPT
	for (i = 0; i < MAX_GPIOA_LINE; i++) {
		gpio_a_isr[i] = NULL;
	}
	for (i = 0; i < MAX_GPIOB_LINE; i++) {
		gpio_b_isr[i] = NULL;
	}

	/* Clear All Interrupt Status */
	HAL_GPIOA_CLEAR_INTERRUPT(0xFFFFFFFF);
	HAL_GPIOB_CLEAR_INTERRUPT(0xFFFFFFFF);
	str8100_set_interrupt_trigger(INTC_GPIO_EXTERNAL_INT_BIT_INDEX, INTC_EDGE_TRIGGER, INTC_RISING_EDGE);
	ret = request_irq(INTC_GPIO_EXTERNAL_INT_BIT_INDEX, str8100_gpio_irq_handler, 0, "str8100_gpio", 0);
	if (ret < 0) {
		printk("request_irq fail : %d \n", ret);
		return 0;
	} else {
		printk("GPIO interrupt handler install ok. \n");
	}
#endif
#ifdef STR8100_GPIO_INTERRUPT_TEST
	str8100_gpio_a_set_edgeintr(&str8100_gpio_intr_test, PIN_TRIG_SINGLE, PIN_TRIG_RISING, 0);
	str8100_gpio_a_set_levelintr(&str8100_gpio_intr_test, PIN_TRIG_HIGH, 1);
#endif

	return 0;
}	

void __exit str8100_gpio_exit(void)
{
	free_irq(INTC_GPIO_EXTERNAL_INT_BIT_INDEX, 0);
}

module_init(str8100_gpio_init);
module_exit(str8100_gpio_exit);

MODULE_LICENSE("GPL");
