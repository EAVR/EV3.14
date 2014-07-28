#!/bin/sh

if [ $1 == "write" ]; 
then
  umount /mnt/tmpdisk/ > /dev/null 2>&1;
  echo "Saving data...";

  mount -t tmpfs tmpfs /mnt/tmpdisk/ -o size=4M
  cd /mnt/ramdisk && tar -czf /mnt/tmpdisk/data.tgz ./* > /dev/null 2>&1
  # add splash-animation
  mtd_debug erase /dev/mtd4 0x0 0x00350000 > /dev/null 2>&1;
  dd if=/mnt/tmpdisk/data.tgz of=/dev/mtd4 bs=64k > /dev/null 2>&1;
  umount /mnt/tmpdisk/
  echo "Done..."
fi

