
BASEINCLUDE ?= /lib/modules/`uname -r`/build

slab_lab-objs := slab.o

obj-m	:= slab_lab.o
all :
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


