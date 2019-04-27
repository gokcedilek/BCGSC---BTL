#!/bin/bash
IFS=$'\n'
#$1: is there a sequence in here that is NOT in $2?
#$2: read $1, output if a sequence in $1 is not in $2
#iow: $1 is the subset of $2

#filename=$(basename ./$1)
#echo "The sequences that are only in ${filename%%.*}:"

#while read id;
#do
#	read seq
#	if ! grep -Fxq "$seq" $2
#	then
#		echo "$id"
#		echo "$seq"
#	fi
#done < $1 #read from 1, print the ones that are NOT in 2!!

#used in another script- do not modify!
#$2: hits
#$1: 20subjectAMPs
count=0
while read id;
do
	if ! grep -Fxq "$id" $2 #if a particular subject AMP is not in hits
	then
		echo "There is a subject AMP that is not in Jhmmer hits:"
		echo "${id}"
		count=$((count+1))
	fi
done<$1
#echo "${count}"
if count -ne 0
then
	echo "There are $count subject AMPs that are not in Jhmmer hits"
else
	echo "All subject AMPs are in Jhmmer hits!"
fi

