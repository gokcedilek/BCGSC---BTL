#!/bin/bash
IFS=$'\n'
t=$(printf '\t')
#this script orders the Physlr steps for time and/or memory graphs based on the information extracted from the logfile
log=$1
unordered=$2 #we need both the logfile and the unordered time/memory files --> in order to match the steps

#this condition (grep "gtime" | grep -v "paf.gz") only works with the current version of the Makefile --subject to change
for line in $(cat $log | grep "gtime" | grep -v "paf.gz")
do	
	makestep=$(echo "$line" | grep -oP '../[a-zA-Z]+/physlr-* *[a-zA-Z]+-*[a-zA-Z]*-*[a-zA-Z]*' | awk '{ if(NF==2) {print $2;} else { split($0,a,"physlr-"); print a[2]}}')
	grep "${makestep}" $unordered >> ${unordered}.ordered #write into the same directory!!
done
