BASEINCLUDE ?= /lib/modules/`uname -r`/build

testsyscall_issue-objs := test_syscall_issue.o 

obj-m	:=   testsyscall_issue.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -f *.ko;


