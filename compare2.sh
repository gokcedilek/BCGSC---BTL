#!/bin/bash
IFS=$'\n'

#compare two input FASTA files (based on IDs):
if [[ "$1" == "/dev/fd/63" ]]
then
	cp $1 temp_file
	in_1=temp_file
else
	in_1=$1
fi #if condition handles the temporary file input!

in_2=$2
in_1_name=$(basename "$in_1")
in_2_name=$(basename "$in_2")
echo "The sequences that are included in "$in_1_name" but not included in "$in_2_name" are: "
while read id;
do
	read seq
	if ! grep -Fxq "$id" "$in_2"
	then
		echo "${id}"
		echo "${seq}"
	fi
done< "$in_1"

echo "The sequences that are included in "$in_2_name" but not included in "$in_1_name" are: "
while read id;
do
	read seq
	if ! grep -Fxq "$id" "$in_1"
	then
		echo "${id}"
		echo "${seq}"
	fi
done< "$in_2"



