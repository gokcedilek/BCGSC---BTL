#!/bin/bash
IFS=$'\n'

for file in $(ls ../blastp_queryapis.new_unique/* | grep "prot" | grep -v "muscle")
do #we meed /* at the end to keep the full path in the filenames (files)!!!
	#echo $file
	name=$(basename $file)
	#echo $name
	#echo "${name}.muscle"
	muscle -in $file -out ${name}.muscle -clw
	#/gsc/btl/linuxbrew/bin/muscle -in $file -out ${name}.muscle -clw
done
