BASEINCLUDE ?= /lib/modules/$(shell uname -r)/build

list-crash-objs := list_crash.o 
KBUILD_CFLAGS +=-g


obj-m	:=   list-crash.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


