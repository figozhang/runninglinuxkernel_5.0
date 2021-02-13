
BASEINCLUDE ?= /lib/modules/`uname -r`/build
ccflags-y :=-g

rcu-objs := rcu_test.o 

obj-m	:=   rcu.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


