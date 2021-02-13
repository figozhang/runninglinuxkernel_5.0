
BASEINCLUDE ?= /lib/modules/`uname -r`/build

mydevdemo_mmap-objs := mydev_mmap.o 

obj-m	:=   mydevdemo_mmap.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


