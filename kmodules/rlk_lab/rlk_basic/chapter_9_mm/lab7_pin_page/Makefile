
BASEINCLUDE ?= /lib/modules/`uname -r`/build

mydevdemo-pin-page-objs := mydev_pin_page.o 

obj-m	:=   mydevdemo-pin-page.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


