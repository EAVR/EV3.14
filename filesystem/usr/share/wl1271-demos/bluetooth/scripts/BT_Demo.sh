#!/bin/sh
#/***************************************************************************
# * \file    BT_Demo.sh
# *
# * \brief   Master script to demonstrate the BT features
# *
# * \author  Sinoj@Mistral
# *
# * \version 01a, 09Dec10, created 
# ***************************************************************************/

# Script File name definitions
BT_SCAN=BT_Inquiry.sh
BT_DEV_BRWOSE=BT_Get_Device_Capabilies.sh
BT_OPP_PULL=BT_OPP_Remote_Push.sh
BT_OPP_PUSH=BT_OPP_Push.sh
BT_FTP_PULL=BT_FTP_Pull.sh
BT_FTP_PUSH=BT_FTP_Push.sh
BT_A2DP_INIT=BT_A2DP_Init.sh
BT_A2DP_PLAY=BT_A2DP_Play.sh
BT_PAIR=BT_Pair.sh
BT_UNPAIR=BT_UnPair.sh
BT_HFP_INIT=BT_HFP_Init.sh
BT_HFP_PLAY=BT_HFP_Play.sh
BT_INIT=BT_Init.sh
BT_CLEAN=BT_Exit.sh

GALLERY=/usr/share/wl1271-demos/bluetooth/gallery
FTP_STORE_PATH=$GALLERY

echo "Starting BT Demo ......."


hciconfig hci0 &> /dev/null
if [ "$?" -ne 0 ]; then
. ./$BT_CLEAN
echo "

!!!!!!!!!!! Start from the MAIN MENU !!!!!!!!!!!!!!!!!"
. ./$BT_INIT
sleep 1
fi


stty erase ^H

while [ 1 ]
do

    echo  -n "
+++++++++++++++++++++++++++++++++++++++++++++++++++++
               MAIN NENU
+++++++++++++++++++++++++++++++++++++++++++++++++++++
    1 - BT Scan Remote Devices
    2 - BT Get capabilities of Remote Device
    3 - OPP  Test
    4 - FTP  Test
    5 - HID  Test
    6 - HSP  Test
    7 - A2DP Test
    8 - Pair Remote Device
    9 - Unpair Remote Device
    10 - Exit Without BT Shut Down
    11 - Exit With BT Shut Down
====> "

    read MAIN_OPT

    case "$MAIN_OPT" in
    1)
#       BT Scan Remote Devices
        . ./$BT_SCAN
    ;;


    2)
#       BT Get capabilities of Remote Device
        . ./$BT_DEV_BRWOSE
    ;;


    3)
#         BT-OPP Tests
        echo "
        Help:
            Before running OPP-Test, ensure that we have the following mandatory inputs;
            a. BD Address of Remote BT-Device (Use Option-1 of MAIN MENU).
            b. Channel Number of the OPP service of that device (Use Option-2 of MAIN MENU).
        "

        echo " 1. Continue..."
        echo " 2. Goto MAIN MENU"
	echo -n "====> "
        read OPP_OPT

	case "$OPP_OPT" in
	1)
	;;

	*)
	    continue
	;;
	esac

        echo "==== Options for OPP test ===="
        echo "1 - Object Push from Remote-Device to WL1271-Host"
        echo "2 - Object push from WL1271-Host to Remote-Device"
	echo "3 - Goto MAIN MENU"
	echo -n "====> "
        read OPP_OPT

        case "$OPP_OPT" in
        1)
	    . ./$BT_OPP_PULL
        ;;

        2)
	    . ./$BT_OPP_PUSH
        ;;

	*)
	    continue
	;;

        esac
    ;;


    4)
#       BT-FTP Tests
	echo "==== Options for FTP tests ===="
	echo "1 - FTP from Remote Device to OMAP3EVM"
	echo "2 - FTP from OMAP3EVM to remote device"
	echo "3 - Goto MAIN MENU"
	echo -n "====> "
	read FTP_OPT

	case "$FTP_OPT" in
	1)
	    . ./$BT_FTP_PULL
	;;

	2)
	    echo "
	    Help:
		Before running FTP-Test, ensure that we have the following mandatory inputs;
	        a. BD Address of Remote BT-Device (Use Option-1 of MAIN MENU).
	        b. Channel Number of the FTP service of that device. (Use Option-2 of MAIN MENU).
	    "

	    echo "1. Continue..."
	    echo "2. Goto MAIN MENU"
	    echo -n "====> "
	    read FTP_OPT

	    case "$FTP_OPT" in
	    1)
	    ;;

	    *)
	        continue
	    ;;
	    esac

	    . ./$BT_FTP_PUSH
	;;

	*)
	    continue

	esac
    ;;


    5)
#       BT-HID Tests
	echo "!!!! HID is not supported !!!!"
        sleep 2
    ;;


    6)
#       BT-HFP Tests
        echo "!!!! HFP/HSP is not supported !!!!"
        sleep 2
    ;;


    7)
#       BT-A2DP Tests
        echo "
        Help:
	    Before running A2DP-Test, ensure that we have the following mandatory inputs;
            a. BD Address of Remote BT-Device (Use Option-1 of MAIN MENU).
            b. The remote A2DP device is paired with us (Use Option-8 of MAIN MENU).
        "

        echo "1. Start Audio..."
        echo "2. Stop Audio..."
        echo "3. Goto MAIN MENU"
        echo -n " ====> "
        read OPT

        case "$OPT" in
	1)
            . ./$BT_A2DP_INIT 
            . ./$BT_A2DP_PLAY
            sleep 5

            echo ""                               
            echo "2. Stop Audio..."                                                 
            echo "3. Goto MAIN MENU"                                                
            echo -n " ====> "                                                       
            read OPT                                                                
                                                                                
            case "$OPT" in                                                          
            2)                                                                      
                killall -15 aplay                                                   
                sleep 1                                                             
            ;;                           
            
            3)                                                                      
                continue                                                            
            ;;
           
            *)
                echo ""
            ;;
                                                                       
            esac           
	;;

        2)
            killall -15 aplay
            sleep 1
        ;;

        *)
            continue
        ;;
        esac

    ;;


    8)
#       Pairing Tests
        echo "
        Help:

            Important: Ensure that the remote device is in pairing/scanning mode.

	    Before running Pairing-Test, ensure that we have the following mandatory inputs;
            a. BD Address of Remote BT-Device (Use Option-1 of MAIN MENU)
        "

        echo "1. Continue..."
        echo "2. Goto MAIN MENU"
        echo -n "====> "
        read OPT

        case "$OPT" in
	1)
	;;

        *)
            continue
        ;;
        esac

        . ./$BT_PAIR
    ;;


    9)
#       UnPairing Tests
        echo "
        Help:
	    Before running UnPair-Test, ensure that we have the following mandatory inputs;
            a. BD Address of Remote BT-Device (Use Option-1 of MAIN MENU)
        "

        echo "1. Continue..."
        echo "2. Goto MAIN MENU"
        echo -n "====> "
        read OPT

        case "$OPT" in
	1)
	;;

        *)
            continue
        ;;
        esac

        . ./$BT_UNPAIR
    ;;


    10)
#	echo " Soft Exit.......";
	exit 
    ;;

    11)                                                        
#shut down BT                                             
        . ./$BT_CLEAN                                         
                                                              
        echo " Soft Exit.......";                             
        sleep 2                                               
        exit                                                  
    ;;        
    esac

done
echo " "
