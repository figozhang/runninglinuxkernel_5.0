BASEINCLUDE ?= /lib/modules/$(shell uname -r)/build

oops-objs := ramdisk-driver.o 
KBUILD_CFLAGS +=-g -O0


obj-m	:=   ramdisk_driver.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

install:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules_install;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


