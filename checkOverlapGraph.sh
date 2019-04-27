#!/bin/bash
set -eu
IFS=$'\n'
t=$(printf '\t')
py=$1
cpp=$2

while read line; do
	node1=$(echo $line | cut -f1)
	node2=$(echo $line | cut -f2)
	edge=$(echo $line | cut -f3)
	pat1="${node1}${t}${node2}${t}${edge}"
	pat2="${node2}${t}${node1}${t}${edge}"
	if grep -qE "${pat1}|${pat2}" <(cat $cpp | grep -oP '[a-zA-Z]+\-[0-9]+\t[a-zA-Z]+\-[0-9]+\t[0-9]+'); then
		continue
	else
		echo "${line} is in the first file but not in the second file!" >&2
		exit 1
	fi
done< <(cat $py | grep -oP '[a-zA-Z]+\-[0-9]+\t[a-zA-Z]+\-[0-9]+\t[0-9]+')
		
#	#don't split "line" itself!; use "echo" and "$0"
#	node=$(echo $line | awk '{split($0, arr, "\t"); print arr[1]}')
#	n=$(echo $line | awk '{split($0, arr, "\t"); print arr[2]}')
#	echo "${node}"
#	echo "${n}"
#	grep 
#done< <(grep -P '[a-zA-Z]+\-[0-9]\t[0-9]+' $py)
#'[a-zA-Z]+\-[0-9]+\s+[a-zA-Z]*\-*[0-9]*\s*[0-9]+'
#cut -f1 <(grep -P '[a-zA-Z]+\-[0-9]\t[0-9]+' $py) | sort -h > nodes.sort
#cut -f2 <(grep -P '[a-zA-Z]+\-[0-9]\t[0-9]+' $py) | sort -n > ns.sort
