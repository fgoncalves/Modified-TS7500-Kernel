#
# Makefile for the linux kernel.
#

# Object file lists.

obj-y			:= str9100_debug.o str9100_setup.o str9100_timer.o str9100_intc.o str9100_counter.o str9100_misc.o
obj-m			:=
obj-n			:=
obj-			:=

obj-$(CONFIG_PCI) += str9100_pci.o
obj-$(CONFIG_STR9100_GPIO)	+= str9100_gpio.o
obj-$(CONFIG_STR9100_RTC)	+= str9100_rtc.o