#!/bin/sh

OUTFILE=OpenSSLspeedResults.txt


OPENSSL=openssl


$OPENSSL version

echo "Running OpenSSL Speed test.  "
echo "This test can take several minutes.  Please Wait..."


time -v $OPENSSL speed -evp aes-128-cbc >> $OUTFILE 2> $OUTFILE
time -v $OPENSSL speed -evp aes-192-cbc >> $OUTFILE 2>> $OUTFILE
time -v $OPENSSL speed -evp aes-256-cbc >> $OUTFILE 2>> $OUTFILE
time -v $OPENSSL speed -evp des-cbc >> $OUTFILE 2>> $OUTFILE
time -v $OPENSSL speed -evp des3 >> $OUTFILE 2>> $OUTFILE
time -v $OPENSSL speed -evp sha1 >> $OUTFILE 2>> $OUTFILE
time -v $OPENSSL speed -evp md5 >> $OUTFILE 2>> $OUTFILE

egrep 'Doing|User|System|Percent|Elapsed' $OUTFILE
echo "Summary above.  Complete results written to ${OUTFILE}"


