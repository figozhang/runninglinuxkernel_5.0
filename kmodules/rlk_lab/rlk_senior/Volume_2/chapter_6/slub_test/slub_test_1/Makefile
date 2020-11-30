
BASEINCLUDE ?= /lib/modules/$(shell uname -r)/build
slub1-objs := slub_test.o 

obj-m	:=   slub1.o
all : 
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) clean;
	rm -f *.ko;


