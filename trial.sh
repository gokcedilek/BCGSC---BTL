#!/bin/bash
IFS=$'\n'
jh_out=$1
seq_ids=$2
for id in $(cat $seq_ids)
do
	#echo "${id}"
	grep -E "Query:|${id}" $1
done

grep -E "Query:|
#for line in $(cat $jh_out)
#do
#	grep -E "Query:|" $1 
