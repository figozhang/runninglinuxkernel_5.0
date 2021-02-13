
BASEINCLUDE ?= /lib/modules/`uname -r`/build
slub2-objs := slub_test.o 

obj-m	:=   slub2.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


