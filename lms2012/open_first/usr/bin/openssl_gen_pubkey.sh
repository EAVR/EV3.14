#!/bin/sh

CERTFILE=matrixcert.pem
PUBKEY=pubkey.pem


OPENSSL=openssl

echo -e "\nGenerating Public Key from ${CERTFILE}"


if [ ! -r $CERTFILE ]
then
	echo "Certificate does not exist.  Generate certificate first before generating a public key"
	exit
else
	$OPENSSL rsa -in $CERTFILE -pubout > $PUBKEY
	echo -e "\nPublic Key written to ${PUBKEY}\n"
fi

cat $PUBKEY

