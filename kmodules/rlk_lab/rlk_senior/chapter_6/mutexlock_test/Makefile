BASEINCLUDE ?= /lib/modules/$(shell uname -r)/build

mutexlock-objs := mutexlock_test.o 

obj-m	:=   mutexlock.o
all : 
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) clean;
	rm -f *.ko;


