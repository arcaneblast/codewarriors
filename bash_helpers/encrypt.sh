#!/bin/bash

# echo "hehe" | gpg --batch --passphrase-fd 0  -o myfile.gpg --cipher-algo AES256 --symmetric myfile
# Please enter the file path first then the passpharse
rm /tmp/my_tempo_omg &> /dev/null
echo "$2" | gpg --batch --passphrase-fd 0  -o /tmp/my_tempo_omg --cipher-algo AES256 --symmetric $1 &> /dev/null
STATUS=$?
if [ $STATUS -ne 0 ]
then
	exit $STATUS
fi

rm $1 &> /dev/null
mv /tmp/my_tempo_omg $1 &> /dev/null
exit $?
