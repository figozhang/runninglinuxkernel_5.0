
BASEINCLUDE ?= /lib/modules/$(shell uname -r)/build
kasan-test-objs := kasan_test.o 

obj-m	:=   kasan-test.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


