#!/bin/sh

if [ -e /var/volatile/run/tiwlan0 ]; then
	$1 -p /var/volatile/run -i tiwlan0 
else
	echo "wpa_supplicant not running"
fi
