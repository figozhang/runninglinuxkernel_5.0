BASEINCLUDE ?= /lib/modules/$(shell uname -r)/build

mydev-mmap-objs := mydev_mmap.o 
KBUILD_CFLAGS +=-g
cflags-y +=  -Wno-unused-value -Wno-unused-label


obj-m	:=   mydev-mmap.o
all : 
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) clean;
	rm -f *.ko;


