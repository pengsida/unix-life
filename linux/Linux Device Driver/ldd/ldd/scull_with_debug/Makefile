obj-m: scull.o
KERNEL_DIR = /lib/modules/$(shell uname -r)/include

# 决定是否输出debug信息
IS_PRINT_DEBUG = y
ifeq($(IS_PRINT_DEBUG), y)
    CFLAGS += -O 0 -g -D PRINT_DEBUG
else
    CFLAGS += -O 2
endif

modules:
    make $(CFLAGS) -C $KERNEL_DIR M=$(shell pwd) modules

clean:
    rm -f $(shell ls | grep ".*mod\.c")
    rm -f $(shell ls | egrep -v "(.*\.c|.*\.h|Makefile|.*\.sh)")
