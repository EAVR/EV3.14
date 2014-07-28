#!/bin/sh
#/***************************************************************************
# * \file    BT_Init.sh
# *
# * \brief   Script to Initialize the BT
# *
# * \author  Sinoj@Mistral
# *
# * \version 01a, 09Dec10, created
# ***************************************************************************/

FTP_STORE_PATH=$GALLERY

echo "
/*
 * #################################################
 *                  Initialize BT
 * #################################################
 */
 "
rm   /var/run/messagebus.pid &> /dev/null
rm   /var/run/dbus/pid &> /dev/null
sleep 1
dbus-daemon --system &> /dev/null &
sleep 2
bluetoothd -n &> /dev/null &
#insmod `find /lib/modules/ -name "gpio_en.ko"`
sleep 2
hciattach /dev/ttyS2 texas 2000000 & 
sleep 10
hciconfig hci0 piscan &> /dev/null
agent --path /org/bluez/agent 1234 &> /dev/null &
sleep 2
sdptool add OPUSH &> /dev/null
obexftpd -c $FTP_STORE_PATH -b &> /dev/null &
