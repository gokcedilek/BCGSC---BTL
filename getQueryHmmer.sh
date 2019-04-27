#!/bin/bash
IFS=$'\n'

if [[ "$#" -ne 2 ]]
then
echo "USAGE: $(basename $0) <Jackhmmer output file, Subject IDs to find with sequences>" 1>&2
	exit 1
fi

in=$1
subjects=$2

#awk -F "\n" 'BEGIN{ RS="Query:" } /$varAY082691.1_prot_AAL92482.1_1/ {print $1}' $in



#if that subject is included, print the query name ($1)
#set RS WITH BEGIN AS QUERY: AND SET FS AS NEWLINE
if [[ -e temp.txt ]]
then
	rm temp.txt
fi
while read ID;
do
	read seq;
	check=$(echo "${ID}"| awk -F "|" '{print $2}'| awk '{print $1}') #awk normally takes a file but we input a word with piping! {cut lcl part; cut the rest of the ID line}
	#echo "${check}" >> temp.txt
	echo "${ID}" >> temp.txt
	#get queries(query IDs) from the JH output file
	#the important part is: $0 ~ var does assign the variable as the pattern!!! you don't need /$0/ again!!!
	awk -F "\n" -v var="$check" 'BEGIN{ RS="Query:" }; $0 ~ var {print $1}' $in >> temp.txt #couple of things: field sep "\n", record sep: "Query:", pattern is the "check" variable!!! --> look for this pattern between records and if found, print the first line of the record which is the query ID!
done<$subjects #the novel_sequences folder holding 8 novel IDs + seq.s
