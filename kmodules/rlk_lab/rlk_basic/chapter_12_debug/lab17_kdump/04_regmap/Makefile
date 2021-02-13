BASEINCLUDE ?= /lib/modules/$(shell uname -r)/build

regmap-objs := regmap_test.o 
KBUILD_CFLAGS +=-g
cflags-y +=  -Wno-unused-value -Wno-unused-label


obj-m	:=   regmap.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


