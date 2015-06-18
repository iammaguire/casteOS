#!/bin/sh
set -e
. ./build.sh

if mount | grep /dev/loop0 > /dev/null; then
	umount /dev/loop0
	losetup -d /dev/loop0
fi

if [ -b "/dev/loop0" ]
then
	rm /dev/loop0
fi

if [ -b "floppy.img" ]
then
	rm floppy.img
fi

if [ -b /mnt/osfloppy ]
then
	rm -rf /mnt/osfloppy
fi

dd if=/dev/zero of=floppy.img bs=512 count=2880
mknod /dev/loop0 b 7 0
losetup /dev/loop0 floppy.img
mkdosfs /dev/loop0
mount -t msdos /dev/loop0 /mnt/osfloppy

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/myos.kernel isodir/boot/myos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
set timeout=1
set default=0

menuentry "myos" {
	multiboot /boot/myos.kernel
}
EOF
grub-mkrescue -d /usr/lib/grub/i386-pc/ -o myos.iso isodir/

