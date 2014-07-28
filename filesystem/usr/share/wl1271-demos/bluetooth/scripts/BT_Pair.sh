#!/bin/sh
#/***************************************************************************
# * \file    BT_Pair.sh
# *
# * \brief   Script to demonstrate BT pairing with remote device 
# *
# * \author  Sinoj@Mistral
# *
# * \version 01a, 09Dec10, created
# ***************************************************************************/

echo "
/*
 * ########## BT-Pair ########
 * Pair to a remote BT device
 * ###########################
 */
 "

BDADDR_FILE=bd_addr.txt


echo -n "Enter the BD Addr of remote Device to Pair
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


echo "Pairing with $BD_ADDR, Please wait.."
echo ""
dbus-send --system --print-reply --dest=org.bluez  $BT_ADAPTER org.bluez.Adapter.RemoveDevice objpath:$BT_ADAPTER/dev_$BD_ADDR_XX &> /dev/null
sleep 2
dbus-send --system --print-reply --dest=org.bluez  $BT_ADAPTER org.bluez.Adapter.CreatePairedDevice string:$BD_ADDR objpath:/org/bluez/agent string:DisplayYesNo &> /dev/null
if [ "$?" -ne 0 ]; then
   sleep 2
   dbus-send --system --print-reply --dest=org.bluez  $BT_ADAPTER org.bluez.Adapter.RemoveDevice objpath:$BT_ADAPTER/dev_$BD_ADDR_XX &> /dev/null
   sleep 2
   dbus-send --system --print-reply --dest=org.bluez  $BT_ADAPTER org.bluez.Adapter.CreateDevice string:$BD_ADDR &> /dev/null
   dbus-send --system --print-reply --dest=org.bluez  $BT_ADAPTER/dev_$BD_ADDR_XX org.bluez.Device.DiscoverServices string: &> /dev/null
   if [ "$?" -ne 0 ]; then
        echo ""
        echo "Pairing failure ..."
   else
        echo ""
        echo "Pairing success ..."
   fi
else
   echo ""
   echo "Pairing success ..."
fi

