#!/bin/sh
#/***************************************************************************
# * \file    A2DP_IN_LOOP.sh
# *
# * \brief   Script to run A2DP multiple times for testing
# *
# * \author  Sinoj@Mistral
# *
# * \version 01a, 09Dec10, created
# ***************************************************************************/

$GALLERY=/usr/share/wl1271-demos/bluetooth/gallery
echo ""
echo ""
echo "A2DP streaming.. repeat it for several times... till headset dies !!"

echo "Is the headset paired with? y/n"
read PAIR
if [ "$PAIR" != "y" ] ; then
exit
fi

echo "Enter number of times to play:"
read COUNT

chmod 777 BT_A2DP_Init.sh
. ./BT_A2DP_Init.sh

IDX=0

while [ "$IDX" -ne "$COUNT" ] 
do
 IDX=`expr $IDX + 1`
 echo "Count = "$IDX""
 aplay -Dplug:bluetooth $GALLERY/Pop.wav 
 sleep 2
done

