#!/bin/sh

#  scull_unload.sh
#  ldd
#
#  Created by pengsida on 2017/4/30.
#  Copyright © 2017年 pengsida. All rights reserved.

# 1. 卸载模块
# 2. 删除设备文件

module="scull"
device="scull"
nr_device=4

rmmod $module

rm -f /dev/${device}[0-$[$nr_device-1]]
