obj-m := hello.o
KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

modules:
$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
