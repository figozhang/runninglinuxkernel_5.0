BASEINCLUDE ?= /lib/modules/`uname -r`/build

rb_test-objs := rbtree_test.o 

obj-m	:=   rb_test.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


