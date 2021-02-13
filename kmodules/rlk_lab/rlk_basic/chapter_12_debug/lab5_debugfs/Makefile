BASEINCLUDE ?= /lib/modules/`uname -r`/build

debugfs-test-objs := debugfs_test.o 

obj-m	:=   debugfs-test.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


