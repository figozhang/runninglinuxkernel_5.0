
BASEINCLUDE ?= /lib/modules/`uname -r`/build

allocmm-objs := alloc_mm.o

obj-m	:=   allocmm.o
all :
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


