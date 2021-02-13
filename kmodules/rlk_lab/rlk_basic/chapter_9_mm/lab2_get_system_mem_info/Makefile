
BASEINCLUDE ?= /lib/modules/`uname -r`/build

mm_info-objs := get_mm_info.o

obj-m	:=   mm_info.o
all :
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


