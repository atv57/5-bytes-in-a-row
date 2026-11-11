#!/bin/busybox sh

/bin/busybox --install -s /bin
mkdir -p /proc /sys
mount -t proc proc /proc
mount -t sysfs sys /sys

PARAM=/sys/module/jitterentropy_rng/parameters/debug_fake_failures

# Newer kernels block AF_ALG RNG access by default.
echo 0 > /proc/sys/crypto/af_alg_restrict

echo "=== fips_enabled = $(cat /proc/sys/crypto/fips_enabled)"

echo "=== Test 1: normal read"
/jent_read

echo "=== Test 2: one permanent failure"
echo 1 > $PARAM
/jent_read

echo "=== Test 3: five permanent failures in a row"
echo 5 > $PARAM
/jent_read

echo "=== Done"
dmesg | grep -i "jitter rng"
poweroff -f
