#!/bin/sh
set -e
. ./iso.sh

wmctrl -s 3
qemu-system-$(./target-triplet-to-arch.sh $HOST) -cdrom myos.iso -boot d -hda c.img -m 1024M

if mount | grep /dev/loop0 > /dev/null; then
	echo "Unmounting and deleting loop device..."
	umount /dev/loop0
	losetup -d /dev/loop0
fi
