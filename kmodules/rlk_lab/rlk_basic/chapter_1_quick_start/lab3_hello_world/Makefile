
BASEINCLUDE ?= /lib/modules/`uname -r`/build

hello-world-objs := hello_world.o 

obj-m	:= hello-world.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


