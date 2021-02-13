ccflags-y :=-g
BASEINCLUDE ?=  /lib/modules/`uname -r`/build

kmemleak-test-objs := kmemleak_test.o 

obj-m	:=   kmemleak-test.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


