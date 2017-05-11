obj-m := read_file.o
KERNEL_DIR := /lib/modules/$(shell uname -r)/build

modules:
    make -C $(KERNEL_DIR) M=$(shell pwd) modules

install:
    insmod ./read_file.ko

uninstall:
    rmmod read_file
