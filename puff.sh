#!/bin/bash
IFS=$'\n'

#awk '/^>/' insects_immature.use | grep -c 
in=$1
out=$2
touch $out

for id in $(awk '/^>/' $in)
do
	if [[ "$(grep -c "$id" $out)" -eq 0 ]]  #if it is not added to out yet!
	then
		echo "$id"
		#grep -A 1 --no-group-separator "${id}" $in > $out
	fi
done
