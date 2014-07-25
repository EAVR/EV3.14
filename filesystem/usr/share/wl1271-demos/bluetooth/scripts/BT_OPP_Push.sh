#!/bin/sh
#/***************************************************************************
# * \file    BT_OPP_Push.sh
# *
# * \brief   Script to demonstrate object push over BT-OPP
# *
# * \author  Sinoj@Mistral
# *
# * \version 01a, 09Dec10, created
# ***************************************************************************/

echo "
/*
 * ############################### BT-OPP ###################################
 * Object Push Profile (OPP) is for sending objects such as pictures,
 * virtual business cards etc. It is called push because the transfers
 * are always instigated by the sender (client), not the  receiver (server).
 * ##########################################################################
 */
 "

OPP_FILE_PATH=$GALLERY

echo "--- Object push from WL1271-Host to Remote-Device ---"

echo -n "Enter the BD_ADDR of the remote Device for OPP test
====> "
read BD_ADDR

echo -n "Enter the channel number of OPP service for the $BD_ADDR
====> "
read CH_NUM

echo ""
echo "Local File Available ..."
echo ""
ls $OPP_FILE_PATH

echo -n " Enter the Local file to be transferred
====> "
read LFILE



ls $OPP_FILE_PATH/$LFILE &> /dev/null
if [ "$?" -ne 0 ]; then
    ussp-push $BD_ADDR@$CH_NUM $LFILE $LFILE
else
    ussp-push $BD_ADDR@$CH_NUM $OPP_FILE_PATH/$LFILE $LFILE
fi
