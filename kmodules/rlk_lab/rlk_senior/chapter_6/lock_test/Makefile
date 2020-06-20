
BASEINCLUDE ?= /lib/modules/$(shell uname -r)/build
lock-objs := lock_test.o 

obj-m	:=   lock.o
all : 
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) clean;
	rm -f *.ko;


