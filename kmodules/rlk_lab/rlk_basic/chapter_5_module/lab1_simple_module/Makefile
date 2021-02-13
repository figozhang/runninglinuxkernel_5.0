
#BASEINCLUDE ?= /home/rlk/rlk/runninglinuxkernel_5.0
BASEINCLUDE ?= /lib/modules/`uname -r`/build

CONFIG_MODULE_SIG=n

mytest-objs := my_test.o 

obj-m	:=   mytest.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


