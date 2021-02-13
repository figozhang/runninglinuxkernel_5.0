
BASEINCLUDE ?= /lib/modules/`uname -r`/build

vma_test-objs := vma.o

obj-m	:= vma_test.o
all :
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


