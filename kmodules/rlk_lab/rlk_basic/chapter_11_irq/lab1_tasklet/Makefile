
BASEINCLUDE ?= /lib/modules/`uname -r`/build

mydemo_tasklet-objs := mydemodrv_tasklet.o 

obj-m	:=   mydemo_tasklet.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


