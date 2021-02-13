
BASEINCLUDE ?= /lib/modules/`uname -r`/build

slab_exhau-objs := slab_exhaust.o

obj-m	:= slab_exhau.o
all :
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


