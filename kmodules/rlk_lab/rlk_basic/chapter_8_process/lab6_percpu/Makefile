
BASEINCLUDE ?= /lib/modules/`uname -r`/build

mypercpu-objs := my-percpu.o

obj-m	:=   mypercpu.o
all :
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


