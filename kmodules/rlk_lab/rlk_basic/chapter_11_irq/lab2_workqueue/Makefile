BASEINCLUDE ?= /lib/modules/`uname -r`/build

mydemo_work-objs := mydemodrv_work.o 

obj-m	:=   mydemo_work.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


