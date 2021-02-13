#
# Makefile for the BenOS
#

arm64-obj-y		:= mm.o entry.o traps.o irq.o

obj-y   += $(arm64-obj-y)
obj-m   += $(arm64-obj-m)
head-y  := head.o
extra-y	:= $(head-y) benos.lds
