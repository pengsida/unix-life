#!/bin/bash

# System Prerequisites
sudo apt-get install build-essential python-dev libssl-dev python-pip libmysqlclient-dev libxml2-dev libxslt-dev libpq-dev git git-review libffi-dev gettext ipmitool psmisc graphviz libjpeg-dev

expect -c 'set timeout 5; expect [sudo] { send "p1111111\r" }'
expect -c 'set timeout 10; expect Y/n { send "yes\r" }'
# Python Prerequisites
sudo pip install -U virtualenv -i http://mirrors.aliyun.com/pypi/simple
sudo pip install -U tox -i http://mirrors.aliyun.com/pypi/simple

# Running Unit Tests Locally
git clone http://git.trystack.cn/openstack/ironic
cd ironic
# run the unit tests under py27 and also run the pep8 tests
tox -epy27 -epep8
