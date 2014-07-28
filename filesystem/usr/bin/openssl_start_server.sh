#!/bin/sh

CERTFILE=matrixcert.pem

OPENSSL=openssl


echo -e "\nStarting SSL-enabled web server"


if [ ! -r $CERTFILE ]
then
	echo "Certificate does not exist.  Generate certificate before starting server."
	exit
fi

$OPENSSL s_server -cert $CERTFILE -www &
