#!/bin/bash

#$1: first file to compare
#$2: second file to compare

echo "The sequences that are common:"
#cat <(grep -f $1 $2)
#but we need to compare certain lines of a file
while read id;
do
	read seq
	if grep -Fxq "$id" $1
	then
		echo "$id"
		echo "$seq"
	fi
done < $2
