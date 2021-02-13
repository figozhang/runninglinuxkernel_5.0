
BASEINCLUDE ?= /lib/modules/`uname -r`/build

kthread_test-objs := kthread.o

obj-m	:=   kthread_test.o
all :
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


