
BASEINCLUDE ?= /lib/modules/`uname -r`/build
slub4-objs := slub_test.o 

obj-m	:=   slub4.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


