#!/bin/sh
#/***************************************************************************
# * \file    BT_A2DP_Init.sh
# *
# * \brief   Script to Initialize A2DP
# *
# * \author  Sinoj@Mistral
# *
# * \version 01a, 09Dec10, created
# ***************************************************************************/

echo "
/*
 * ################################# A2DP ###################################
 * Advanced Audio Distribution Profile (A2DP) defines how the high quality 
 * audio can be streamed from one device to another over Bluetooth connection. 
 * ##########################################################################
 */
"

echo -n "Enter the BD Address of the A2DP device
====> "
read BD_ADDR

echo " 
pcm.!bluetooth {
    type bluetooth
    device "$BD_ADDR"
    }
" > /etc/asound.conf

echo "'/etc/asound.conf' got updated, ensure that we have paired with the 
      device $BD_ADDR for A2DP.
"
