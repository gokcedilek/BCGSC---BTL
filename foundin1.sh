#!/bin/bash
#$1: Jhmmer hits --> subject protein sequences
#$2: known AMP sequences
#read from known AMPs and print the ones that are hit with Jhmmer run

while read id;
do
	read seq
	#if [[ $(grep -Fcxq "${seq}" $1) -gt 0 ]]
	#if grep -Fxq "${seq}" $1
	if grep -q "${seq}" $1
	then
	    #there is a sequence in honeybee AMPs that is present in the Jackhmmer hits
	    # count=$((count+1))
		echo "Found"
		echo "${id}" 
		echo "${seq}"
	else
		echo "Not found"
		echo $id
		echo $seq
	fi
done < $2 
