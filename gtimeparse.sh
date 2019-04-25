#!/bin/bash
IFS=$'\n'
t=$(printf '\t')
dir=$1 #this is the data directory that holds the output files
key=$2 #this is the key identifier of the dataset (ie. f1chr2R or f1chr4)
#this script takes the output of a "gtime" command -- extracts the step name(need to be fixed), the process time in either hh:mm:ss or mm:ss (handles both), and the memory usage.
#it later on uses getMinutes.sh to convert this time format to readable minutes
#it writes both time and memory into separate files
#those files then need to be converted into tsv

#ls -d : to not include subdirectories, but simply the directory itself
#$dir* : to access the relative path and not just the file names --> because we need them in "cat $file" --> if we don't use the relative path, we look for the files in this script's directory & can't find them
#for the directory: you need to specify the last backward slash (ex: ~/PycharmProjects/physlrtiming/physlr/data/)

#for the current Physlr makefile, the following grep's work fine, but this subject to change
for file in $(ls -d $dir* | grep "$key" | grep "time" | grep -v "gz")
do
	step=$(cat $file | grep "Command" | grep -oP '../[a-zA-Z]+/physlr-* *[a-zA-Z]+-*[a-zA-Z]*-*[a-zA-Z]*' | awk '{ if(NF==2) {print $2;} else { split($0,a,"physlr-"); print a[2]}}') #step name should be suitable for both python and cpp commands
	clocktime=$(cat $file | grep "Elapsed" | grep -oP '[0-9]*:*[0-9]+:[0-9]+.[0-9]+') #time should be suitable for cases with hh and without hh
	memory=$(cat $file | grep "Maximum" | awk '{print $6}')
	#write to files:
	echo "${step}${t}${clocktime}" >> ${key}.time
	echo "${step}${t}${memory}" >> ${key}.mem
done
. ./getmin.sh ${key}.time

#for file in $(find . -maxdepth 1 -name "*time")
