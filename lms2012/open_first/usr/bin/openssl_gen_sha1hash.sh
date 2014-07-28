#!/bin/sh

DATAFILE=rnddata


OPENSSL=openssl

echo -e "\nGenerate SHA1 Hash"

if [ ! -r $DATAFILE ]
then
	echo "Creating 10M random data file ($DATAFILE)"
	echo "Please Wait..."
	dd if=/dev/urandom of=rnddata bs=1048576 count=10
fi


$OPENSSL dgst -sha1 $DATAFILE > $DATAFILE.sha1
echo -e "\n## Digest of file $DATAFILE is in $DATAFILE.sha1"
cat $DATAFILE.sha1


