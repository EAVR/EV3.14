#!/bin/sh
#/***************************************************************************
# * \file    BT_Pair.sh
# *
# * \brief   Script to demonstrate BT Un-pairing from remote device
# *
# * \author  Sinoj@Mistral
# *
# * \version 01a, 09Dec10, created
# ***************************************************************************/

echo "
/*
 * ######### BT-Unpair ########
 * UnPair to a remote BT device
 * ############################
 */
 "

BDADDR_FILE=bd_addr.txt

echo -n "Enter the BD Addr of remote Device to Unpair
====> "
read BD_ADDR

echo $BD_ADDR > $BDADDR_FILE
BD_ADDR_XX=`sed -e s/:/_/g $BDADDR_FILE`
rm $BDADDR_FILE

BT_ADAPTER=`dbus-send --system --print-reply --dest=org.bluez / \
org.bluez.Manager.DefaultAdapter|awk '/object path/ {print $3}'`

echo $BT_ADAPTER > $BDADDR_FILE
BT_ADAPTER=`sed s/\"//g $BDADDR_FILE`
rm $BDADDR_FILE

dbus-send --system --print-reply --dest=org.bluez  $BT_ADAPTER org.bluez.Adapter.RemoveDevice objpath:$BT_ADAPTER/dev_$BD_ADDR_XX

if [ "$?" -eq 0 ]; then
    echo ""
    echo "Unpairing success ..."
else
    echo ""
    echo "Unpairing failure ..."
fi
