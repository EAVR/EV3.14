#!/bin/sh

DATAFILE=rnddata
OUTFILE=OpenSSLEncDecResults.txt
ERRFILE=timeResults.txt


OPENSSL=openssl

CRYPTOTYPE=aes-256-cbc

echo -e "\nRunning OpenSSL Encryption Decryption (${CRYPTOTYPE})"

if [ ! -r rnddata ]
then
	echo "Creating 10M random data file ($DATAFILE)"
	dd if=/dev/urandom of=rnddata bs=1048576 count=10
fi



## Check OpenSSL version
$OPENSSL version




## Encrypt without HW acceleration
time -v $OPENSSL enc -${CRYPTOTYPE} -salt -in $DATAFILE -out $DATAFILE.swenc -pass pass:crypto 2> $ERRFILE
echo "Encrypting 10M file with no HW acceleration"
egrep 'User|System|Percent|Elapsed' $ERRFILE

## Decrypt without HW acceleration
time -v $OPENSSL enc -d -${CRYPTOTYPE} -in $DATAFILE.swenc  -pass pass:crypto > $DATAFILE.swdec 2> $ERRFILE
echo "Decrypting 10M file with no HW acceleration"
egrep 'User|System|Percent|Elapsed' $ERRFILE

echo -e "\n$DATAFILE=original file; $DATAFILE.swenc=encrypted file; $DATAFILE.swdec=decrypted file"
ls -l $DATAFILE $DATAFILE.swenc $DATAFILE.swdec

echo "Decrypted file is now being compared to the original"
echo "Please wait..."

diff $DATAFILE $DATAFILE.swdec
if [ `echo $?` = "0" ]
then
	echo "### diff of $DATAFILE and $DATAFILE.swdec indicates that they are the same"
else
	echo "### diff of $DATAFILE and $DATAFILE.swdec indicates that they are the different!!  This should not ever happen!!"
fi


