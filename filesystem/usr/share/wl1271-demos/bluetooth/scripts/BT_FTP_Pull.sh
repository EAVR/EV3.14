#!/bin/sh
#/***************************************************************************
# * \file    BT_FTP_Pull.sh
# *
# * \brief   Script to demonstarte FTP over BT
# *
# * \author  Sinoj@Mistral
# *
# * \version 01a, 09Dec10, created
# ***************************************************************************/

echo "
/*
 * ########################### BT-FTP ###############################
 * File Transfer Profile (FTP) provides capability to transfer files
 * and folders in an object store (file system) of another system.
 * ##################################################################
 */
"

sleep 2

echo "

Note:
    - FTP Server started, send file over BT-FTP from the remote
      device. There will not be any notification on EVM when data
      transfer completes. Please ensure that received file presents
      in the path $FTP_STORE_PATH.

"
echo -n "Enter Any key After FTP transfer: "
echo ""
read FTP_KEY
