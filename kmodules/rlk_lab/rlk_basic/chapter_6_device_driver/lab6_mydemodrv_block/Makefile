
BASEINCLUDE ?= /lib/modules/`uname -r`/build

mydemo_misc-objs := mydemodrv_misc.o 

obj-m	:=   mydemo_misc.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


