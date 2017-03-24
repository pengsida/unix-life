#!/bin/bash

git clone http://git.trystack.cn/openstack-dev/devstack.git
sudo ./devstack/tools/create-stack-user.sh

cat > ./devstack/local.conf << EOF
[[local|localrc]]

GIT_BASE=http://git.trystack.cn
NOVNC_REPO=http://git.trystack.cn/kanaka/noVNC.git
SPICE_REPO=http://git.trystack.cn/git/spice/spice-html5.git

FORCE=yes

# Credentials
ADMIN_PASSWORD=p1111111
DATABASE_PASSWORD=p1111111
RABBIT_PASSWORD=p1111111
SERVICE_PASSWORD=p1111111
SERVICE_TOKEN=p1111111
SWIFT_HASH=p1111111
SWIFT_TEMPURL_KEY=p1111111

# Enable Ironic plugin
enable_plugin ironic http://git.trystack.cn/openstack/ironic

# Enable Neutron which is required by Ironic and disable nova-network.
disable_service n-net
disable_service n-novnc
enable_service q-svc
enable_service q-agt
enable_service q-dhcp
enable_service q-l3
enable_service q-meta
enable_service neutron

# Enable Swift for agent_* drivers
enable_service s-proxy
enable_service s-object
enable_service s-container
enable_service s-account

# Disable Horizon
disable_service horizon

# Disable Heat
disable_service heat h-api h-api-cfn h-api-cw h-eng

# Disable Cinder
disable_service cinder c-sch c-api c-vol

# Swift temp URL's are required for agent_* drivers.
SWIFT_ENABLE_TEMPURLS=True

# Create 3 virtual machines to pose as Ironic's baremetal nodes.
IRONIC_VM_COUNT=3
IRONIC_VM_SSH_PORT=22
IRONIC_BAREMETAL_BASIC_OPS=True
DEFAULT_INSTANCE_TYPE=baremetal

# Enable Ironic drivers.
IRONIC_ENABLED_DRIVERS=fake,agent_ssh,agent_ipmitool,pxe_ssh,pxe_ipmitool

# Change this to alter the default driver for nodes created by devstack.
# This driver should be in the enabled list above.
IRONIC_DEPLOY_DRIVER=agent_ipmitool

# The parameters below represent the minimum possible values to create
# functional nodes.
IRONIC_VM_SPECS_RAM=1280
IRONIC_VM_SPECS_DISK=10

# Size of the ephemeral partition in GB. Use 0 for no ephemeral partition.
IRONIC_VM_EPHEMERAL_DISK=0

# To build your own IPA ramdisk from source, set this to True
IRONIC_BUILD_DEPLOY_RAMDISK=False

VIRT_DRIVER=ironic

# By default, DevStack creates a 10.0.0.0/24 network for instances.
# If this overlaps with the hosts network, you may adjust with the
# following.
NETWORK_GATEWAY=10.1.0.1
FIXED_RANGE=10.1.0.0/24
FIXED_NETWORK_SIZE=256

# Log all output to files
LOGFILE=$HOME/temp/devstack.log
LOGDIR=$HOME/temp/logs
IRONIC_VM_LOG_DIR=$HOME/temp/ironic-bm-logs
EOF

./devstack/stack.sh
