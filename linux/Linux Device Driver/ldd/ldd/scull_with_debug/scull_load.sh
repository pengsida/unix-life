#!/bin/bash

#  scull_load.sh
#  ldd
#
#  Created by pengsida on 2017/5/8.
#  Copyright © 2017年 pengsida. All rights reserved.

module="scull"
device="scull"
nr_device=4

lsmod | grep $module && rmmod $module
rm -f /dev/${device}[0-$[${nr_device}-1]]

insmod ./${module}.ko $* || exit 1

major=$(awk '/'${module}'/ {print $1}' /proc/devices)

for((i = 0; i < ${nr_device}; i++))
do
    mknod /dev/${device}$i c $major $i
done

group="staff"

grep -q "^staff" /etc/group || group="wheel"

chgrp $group /dev/${device}[0-$[${nr_device}-1]]
chmod 664 /dev/${device}[0-$[${nr_device}-1]]
