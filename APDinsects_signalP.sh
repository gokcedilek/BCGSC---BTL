#!/bin/bash

#count=$(grep "," $1 | wc -l) counter=0
in_signalp="/projects/amp/honeybee/gdilek/signalP_APDinsects.out"
#while counter -lt count;
#do awk -F "," '/^AP0/ {print $1}' $file >> $2
#	counter=$((counter+1))
#done
#
##while read id;
##do
#	if grep -Fxq "${id}" $2
#	then
#		awk '/^AP0/ {print $1}' $file >> $2
#	fi
##done

awk '/^AP0/ {print $1}' $in_signalp > $1
