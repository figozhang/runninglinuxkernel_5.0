BASEINCLUDE ?= /lib/modules/`uname -r`/build

proc-test-objs := proc_test.o 

obj-m	:=   proc-test.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


