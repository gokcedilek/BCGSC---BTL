#!/bin/bash
IFS=$'\n'

#read from the input file containing duplicated IDs, and write to an output file the unique IDs+seqs and the repeated ones only once

in=$1
name=$(basename $in)
echo $name
uniquename="${name}.unique"
echo $uniquename

if [[ -e "$uniquename" ]]
then
	echo "removing existing"
    rm $uniquename
fi
touch $uniquename
echo "created"

while read id;
do
	read seq;
	if ! grep -Fq "${id}" $uniquename
	then
		echo $id >> $uniquename
		echo $seq >> $uniquename
	fi
done<$in




