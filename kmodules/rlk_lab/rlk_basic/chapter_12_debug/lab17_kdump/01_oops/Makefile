BASEINCLUDE ?= /lib/modules/$(shell uname -r)/build

oops-objs := oops_test.o 
KBUILD_CFLAGS +=-g


obj-m	:=   oops.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


