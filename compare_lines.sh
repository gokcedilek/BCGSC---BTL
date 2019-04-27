#!/bin/bash
IFS=$'\n'

in_1=$1
in_2=$2
in_1_name=$(basename "$in_1")
in_2_name=$(basename "$in_2")

echo "The sequences that are included in "$in_1_name" but not included in "$in_2_name" are: "
while read id;
do
	if ! grep -Fxq "$id" "$in_2"
	then
		echo "${id}"
	fi
done< "$in_1"

echo "The sequences that are included in "$in_2_name" but not included in "$in_1_name" are: "
while read id;
do
    if ! grep -Fxq "$id" "$in_1"
    then
 	   echo "${id}"
   	fi
done< "$in_2"

