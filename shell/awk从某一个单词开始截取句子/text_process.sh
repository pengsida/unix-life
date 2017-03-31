#!/bin/bash

cat $1 | awk -F "openstack" '{
    str=$2
    print ("openstack" str)
}'