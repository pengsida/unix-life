obj-m := scull_psd.o
scull_psd-objs := scull.o

KERNEL_DIR := /lib/modules/$(shell uname -r)/build


# 决定是否输出debug信息
# DEBUG = y
ifeq ($(DEBUG), y)
    EXTRA_CFLAGS += -O -g -DPRINT_DEBUG
else
    EXTRA_CFLAGS += -O2
endif

modules:
    make -C $(KERNEL_DIR) M=$(shell pwd) modules

clean:
    rm -f $(shell ls | grep ".*mod\.c")
    rm -f $(shell ls | egrep -v "(.*\.c|.*\.h|Makefile|.*\.sh)")

install:
    ./scull_load.sh

uninstall:
    ./scull_unload.sh
