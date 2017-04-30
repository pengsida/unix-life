#!/bin/bash

#  scull_load.sh
#  ldd
#
#  Created by pengsida on 2017/4/30.
#  Copyright © 2017年 pengsida. All rights reserved.

# 1. 首先安装模块
# 2. 然后删除原有的设备文件
# 3. 创建新的设备文件 mknod [options] 文件名 类型 主设备号 次设备号
# 4. 更改设备文件所属的组

module="scull"
device="scull"
nr_device=4

insmod ./${module}.ko $* || exit 1

rm -f /dev/${device}[0-${nr_device}]

major=$(cat /proc/devices | grep $device | egrep -o "[0-9]*")

for(( i = 0; i < $nr_device; i++ ))
do
    mknod /dev/${device}$i c $major $i
done

group="staff"
grep -q "^staff" /etc/group || group="wheel"

chgrp $group /dev/${device}[0-$[nr_device-1]]
chmod 644 /dev/${device}[0-$[$nr_device-1]]
