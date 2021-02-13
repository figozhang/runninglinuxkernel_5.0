
BASEINCLUDE ?= /lib/modules/`uname -r`/build

alloc-oom-objs := alloc_oom.o

obj-m	:=   alloc-oom.o
all :
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


