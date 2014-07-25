#!/bin/sh
#/***************************************************************************
# * \file    BT_Inquiry.sh
# *
# * \brief   Script to scan for the remote BT devices
# *
# * \author  Sinoj@Mistral
# *
# * \version 01a, 09Dec10, created
# ***************************************************************************/

echo "
/*
 * ########### BT-Scan #############
 * Scan for the remote BT devices
 * #################################
 */
"
hcitool scan --refresh
