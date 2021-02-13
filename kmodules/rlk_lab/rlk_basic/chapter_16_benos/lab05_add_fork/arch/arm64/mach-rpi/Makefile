#
# Makefile for the BenOS
#

arm64-obj-$(CONFIG_PI_MINI_UART)        += mini_uart.o early_uart.o
arm64-obj-$(CONFIG_DEBUG_ON_EARLY_ASM)        += early_uart.o

arm64-obj-$(CONFIG_PI_PL_UART)        += pl_uart.o
arm64-obj-y        += timer.o

obj-y  += $(arm64-obj-y)
obj-y  += $(arm64-obj-m)
