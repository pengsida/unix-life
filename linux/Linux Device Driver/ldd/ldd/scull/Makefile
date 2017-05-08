obj-m := scull.o
KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

modules:
    $(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
    rm -f $(shell ls | egrep -v "(Makefile|.*\.c|.*\.h|.*\.sh)")
