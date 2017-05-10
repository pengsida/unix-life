#!/bin/bash

#  scull_unload.sh
#  ldd
#
#  Created by pengsida on 2017/5/8.
#  Copyright © 2017年 pengsida. All rights reserved.

module="scull"
device="scull"
nr_device=4

lsmod | grep $module || rmmod $module

rm -f /dev/${device}[0-$[${nr_device}-1]]
