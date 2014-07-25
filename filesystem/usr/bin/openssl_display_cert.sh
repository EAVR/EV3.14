#!/bin/sh

CERTFILE=matrixcert.pem


OPENSSL=openssl

echo -e "\nDisplay Certificate Information"





if [ ! -r $CERTFILE ]
then
	echo "Certificate does not exist."
else
	$OPENSSL x509 -text -in $CERTFILE | more
fi


