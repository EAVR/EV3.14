#!/bin/sh
#/***************************************************************************
# * \file    BT_Exit.sh
# *
# * \brief   Script for Cleaningup BT
# *
# * \author  Sinoj@Mistral
# *
# * \version 01a, 09Dec10, created
# ***************************************************************************/

echo "
/*
 * #################################################
 *                  Cleaning BT
 * #################################################
 */
 "

killall -9 obexftpd                &> /dev/null
killall -9 oppserver               &> /dev/null
killall -9 bluetoothd              &> /dev/null
killall -9 dbus-daemon             &> /dev/null
killall -SIGTERM hciattach         &> /dev/null
#rmmod gpio_en.ko                   &> /dev/null
