#!/bin/bash
cd /root/k
ACCEL=tcg
if [ -w /dev/kvm ]; then ACCEL=kvm; fi
echo "accel=$ACCEL args=$1"
timeout 300 qemu-system-x86_64 -accel $ACCEL -m 1G -smp 2 -nographic -no-reboot \
    -kernel linux/arch/x86/boot/bzImage -initrd initrd.gz \
    -append "console=ttyS0 panic=-1 loglevel=4 $1" > "$2" 2>&1
echo "qemu exit=$?"
