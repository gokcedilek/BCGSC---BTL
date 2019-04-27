#!/bin/bash

var=$(grep 'insects' $1 | tail -n 10) #picked from file (the first argument) the ones containing insects, piped to include the last 10 of them

IFS=$'\n' #default separator to be newline

for line in $var
do
	par_1=$(echo $line | awk -F "," '{print $1}') #pick the first entry on line using , separator -> the first entry is the ID of the sequence and it is separated from the next entry by a ","
	par_2=$(echo $line | awk -F "\"" '{print $2}') #pick everything btw " " and so change the separator be " " because now we want to include the entries separated by commas as a whole so we can no longer use , as the seperator
	par_3=$(echo $line | awk -F "\"" '{print $3}'| awk -F "," '{print $2}') #the logic here is we start picking from the third element (from " perspective) however we don't want to include the comma before the sequence so from , perspective, this means take the second entry (the first entry being the space before the comma)
	#pick the third entry on line using " separator { pick the second field because we start with , }

	echo ">$par_1 $par_2" #write >ID and the information about the AMP
	echo "$par_3" #write the sequence on the next line
done

#in summary this script: i)given a document, filters the lines containing "insects" and as well only takes the last 10 lines of the document that contain "insects" {not very useful in general, ha?} ii) writes the selected parameters in fasta file format in which for each line from the filtered lines, we write the first two parameters on one line and the third parameter on the second line
#Note: we used this script in order to add to another file containing insect AMPs; we called this script with the file to be filtered as the argument and then we used >> to direct the output (the fasta format lines) to add on to another file!
#Diana explained how "awk" works! (-F is an option that allows you to specify a pattern delimiter)

	
