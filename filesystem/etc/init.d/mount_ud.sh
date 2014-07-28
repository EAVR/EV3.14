#!/bin/sh

echo "Mounting Partitions..."

if [ -d "/mnt" ] 
then
     # dd if=/dev/mtd4 of=/mnt/ramdisk/
     #modprobe loop

     #cat /dev/mtd4 > /var/volatile/tmp/userdata.tgz
     #tar -xzf /var/volatile/tmp/userdata.tgz -C /mnt/ramdisk/
     #rm /var/volatile/tmp/userdata.tgz 

     mount -t tmpfs -o size=4M /mnt/tmpdisk/
     cat /dev/mtd4 > /mnt/tmpdisk/userdata.tgz
     tar -xzf /mnt/tmpdisk/userdata.tgz -C /mnt/ramdisk/
     rm /mnt/tmpdisk/userdata.tgz 

     #mount -t ext2 -o loop /var/volatile/tmp/ramdisk /mnt/disk/
     #cp -R /mnt/disk/* /mnt/ramdisk/
else
    echo "Mounting Failed !"
fi
