obj-m := get_current_path.o
KERNEL_DIR := /lib/modules/$(shell uname -r)/build

modules:
    make -C $(KERNEL_DIR) M=$(shell pwd) modules

install:
    insmod ./get_current_path.ko

uninstall:
    rmmod get_current_path
