
BASEINCLUDE ?= /lib/modules/`uname -r`/build

mydemo_error-objs := mydemodrv_error.o 

obj-m	:=   mydemo_error.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


