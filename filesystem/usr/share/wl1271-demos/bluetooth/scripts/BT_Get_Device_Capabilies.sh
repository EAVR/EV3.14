#!/bin/sh
#/***************************************************************************
# * \file    BT_Get_Device_Capabilies.sh
# *
# * \brief   Script to get the BT remote device capabilities
# *
# * \author  Sinoj@Mistral
# *
# * \version 01a, 09Dec10, created
# ***************************************************************************/

echo "
/* 
 * #################### BT-Browse ##################
 * Browse the Device and get the device capabilities
 * #################################################
 */
 "

echo "
Note:
    Capture the following LOG for further reference
"

echo -n "Enter the BD Addr of remote Device for browsing
====> "
read BD_ADDR

echo "
==================== $BD_ADDR ======================


"
sdptool browse $BD_ADDR

if [ `sdptool browse $BD_ADDR | wc -l` -lt 5 ]
then                                          
  sdptool records $BD_ADDR &                        
  sleep 6                                           
fi 

killall sdptool &> /dev/null

echo "
====================================================

"
