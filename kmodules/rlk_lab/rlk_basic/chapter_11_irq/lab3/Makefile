BASEINCLUDE ?= /lib/modules/`uname -r`/build

lab3-test-objs := lab3_test.o 

obj-m	:=   lab3-test.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


