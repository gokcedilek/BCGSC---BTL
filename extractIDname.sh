#!/bin/bash


#awk '/^>/ {print}' not_in_reference.out
#need to pick what is between > and [

#for line in $(cat $1)
#do
#	one=${line#*BEGIN}
#	two=${one%END*}
#
#	echo "$two"
#done

#cat $1 | awk '/^>/ {print $0}'
#| awk -F "BEGIN" '{print $2}'
# | awk -F "END" '{print $1}' > $2
IFS=$'\n'
for line in $(cat $1)
do
	echo $line | awk '/>/ {print $0}'| awk -F ">" '{print $2}'| awk '{print $1}'
	#take everything after ">" and before the first space (space is the default separator, remember?)
	#echo $line | awk '/>/ {print $0}'| awk -F ">" '{print $2}'| awk -F "[" '{print $1}' | awk '{$1=$1;print}'
	#the above also works because it takes btw > and [ however this includes a trailing space, so we remove it by rebuilding the field (awk ignores whitespace by default when assigning fields so we have the leading and trailing spaces trimmed) 
done
#extracts the sequence between > and [ to use in blast match 


