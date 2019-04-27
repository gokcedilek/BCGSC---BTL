#!/bin/bash
if [[ "$#" -ne 2 ]]
then
	echo "Run $(basename $0) with: <Query AMPs> <Subject Proteins>"
	exit 1
fi
query=$1
subj=$2

#read from query, if seq is not in subj, print

while read id;
do
	read seq
	if ! grep -Fxq "$seq" "$subj"
	then
		echo "${id}"
		echo "${seq}"
	fi
done < "$query"
