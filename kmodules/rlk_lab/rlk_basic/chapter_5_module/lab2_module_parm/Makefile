
BASEINCLUDE ?= /lib/modules/`uname -r`/build

mymodule-objs := module_parm.o 

obj-m	:=   mymodule.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


