#!/bin/bash
IFS=$'\n' #bash shell variable for FS (field separator)
#you need to do this BEFORE the for loop because this IFS="\n" is for the for loop!!! it is for correctly interpreting what we perceive a'line' to be; strings separated by a newline character! so you tell the computer to interpret $line as each "line" of the input file (as specified by the first argument)
#you need to enclose commands with $(command)
for line in $(cat $1)
do
	#now we re-set FS with -F ":" because when we are processing each line, we want to separate fields by : rather than space 
	#echo in bash, print in awk
	#BEGIN is for before reading the first argument
	#define that you want to print the items separated by a newline char
	#the reason we need echo $line is that awk needs a file at the end; $line is read from the file
#	echo $line | awk -F ":" 'BEGIN{OFS="\n"}{$1=$1;print $0}'
	#we read each line first, the line is sent to awk,so the awk statement executes on $line variable
	#inside awk, before reading anything, we set the FS to be : and the OFS to be newline, after making these changes, we force the record to be rebuilt with $1=$1 so we can't ignore that part, this doesn't work: echo $line | awk 'BEGIN{FS=":"}{OFS="\n"}{print $0}'without forcing the records to change
	#however this works: echo $line | awk 'BEGIN{FS=":"}{OFS="\n"}{$1=$1} {print}'
#	echo $line | awk 'BEGIN{FS=":"}{OFS="\n"}{$1=$1;print $0}'
#this is wrong!!!:	echo $line | awk 'BEGIN{OFS="\n"}{FS=":"}{$1=$1;print $0}' # so you need to first define FS!
#	echo $line | awk 'BEGIN{FS=":"}{OFS="\n"}{$1=$1;print}'
#	echo $line | awk 'BEGIN{FS=":"}{OFS="\n"}{print $0}'
#	echo $line | awk 'BEGIN{FS=":"}{OFS="\n"}{$1=$1}{print}'
	# print $0 is the same thing as print (shortcut) because $0 means ALL FIELDS and print processes field by field, as defined by 1)FS~IFS 2)OFS with their given characters
	done

#Note: these are equivalent: echo $line | awk -F ":" 'BEGIN{OFS="\n"}{$1=$1;print $0}'    and     echo $line | awk 'BEGIN{FS=":"}{OFS="\n"}{$1=$1;print $0}'
