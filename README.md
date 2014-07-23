EV3.14
======

A modified firmware for the Lego EV3 that allows it to be driven by a RaspberryPi

To use it you will first have to build the project : 
  - install the crosscompiler toolchain, download the Sourcery G++ Lite 2009q1-203 cross-compilation toolchain from 
  https://sourcery.mentor.com/GNUToolchain/package4573/public/arm-none-linux-gnueabi/arm-2009q1-203-arm-none-linux-gnueabi.bin 
  Install it by running the downloaded file. 
  - ensure you have all the packages neede for this project : run the command
      sudo apt-get install u-boot-tools doxygen imagemagick libmagickcore-dev
  - then to build the project go into directory ./lms2012/open_first and run 
      make lms2012 modules programs kernel u-boot doc
      
After this part you have to flash the µ-SD card that goes into the EV3 :
  - in the directory open_first, run the script format_sdcard.sh and follow the instructions.
  - remove and reinsert the SD-card.
  - run the script ~/projects/lms2012/open_first/update_sdcard.sh and follow the instructions
  
You may now insert the SD card into your EV3 and turn it on. The bootloader will detect the presence of a firmware on the SD-card, boot the kernel on the first MSDOS partition, mount the second partition (ext3) as root directory and start the init scripts. 
  
