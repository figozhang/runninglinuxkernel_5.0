
BASEINCLUDE ?= /lib/modules/$(shell uname -r)/build 
KBUILD_CFLAGS +=-g

oops-objs := oops_test.o 

obj-m	:=   oops.o
all : 
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) clean;
	rm -f *.ko;


