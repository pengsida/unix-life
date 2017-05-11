obj-m := get_task_path.o
KERNEL_DIR := /lib/modules/$(shell uname -r)/build

modules:
    make -C $(KERNEL_DIR) M=$(shell pwd) modules

install:
    insmod ./get_task_path.ko

uninstall:
    rmmod get_task_path
