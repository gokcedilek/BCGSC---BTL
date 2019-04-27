#!/bin/bash
IFS=$'\n'


#record separator will be "Done"
#field sep will be "\n"
#for every record, print the lines with "lcl" - exclude lcl and [L=...]
#for every line with "lcl", extract their sequences from the input query file (insects)
#for line in $(cat $1)
#do
#	echo $line | awk 'BEGIN{ RS="Done"; FS="\t";} {print $1}'
#	#get all the lines containing "lcl"-queries; exclude "lcl" part; exclude "[L=...]" part!
#	#echo $line | awk -F "\n" 'BEGIN{ RS="Done" } /lcl/ {print $1}'| awk -F "|" '{print $2}' | awk -F " " '{print $1}'
#	#echo $line | awk -F "\n" 'BEGIN{ RS="Done" } {print $2}' #this is very important!!!! {print $0} would print the whole record; it is the whole record; $1,$2,and so on are the respective "fields" "in/within" the "records" --> in this case we wanted the first line of the record (which is the query ID), so we used $1 for the first field of each record!!!
#done
#
#awk -F "\n|\t" 'BEGIN{ RS="Done" }; {print $1}' $1
#if [[ -e out.txty ]]
#then
#	rm out.txty
#fi
in=$1
#cat $in | awk 'BEGIN{ RS="Done"; FS="\n";} {$1=$1; print $0}'
for line in $(cat $1 | awk 'BEGIN { FS= "\n"; OFS="\n" }; {print}')
do
	#echo $line #this simply prints each line of the file; now we need to distinguish btw the lines using the ">" sign!

	if [[ "$line" == \>* ]] #starts with ">"; but you need to escape the ">" character with "\" so that it won't be misunderstood as smth else (it is a part of the pattern)
	then
		#echo "subject"
		filename=$(echo $line | awk -F ">" '{print $2}' | awk '{print $1}' | awk -F "|" '{print $2}')
		#echo $filename
		seqtk subseq ../novel_sequences.blastp <(echo $line | awk -F ">" '{print $2}') > $filename #because seqtk subseq doesn't take the > sign! 
	else
		echo "query" >> $filename
		seqtk subseq ../insects_immature.use.copy.unique <(echo $line | awk '{print $1}') >> $filename
	fi
done
	
	
	#	if [[ "$line" == "\t*" ]]
#	then
#		echo $line
	#	echo "subject"
		#filename=$(echo $line | awk -F "|" '{print $2}').fa
		#echo $line >> $filename
	#else
	#	echo $line
	#	echo "query"
		#echo $line >> $filename
#	fi
	#echo $line
	#echo $line | awk 'BEGIN { FS=" " }; {$1=$1; print $1}' >> out.txty
	#seqtk subseq ../novel_sequences.blastp <(echo $line | awk '{print $1}') >> out.txty
	#echo $line | awk 'BEGIN { RS="Done" }; {$1=$1; print}' | awk '{print $1}'
	#echo "break" 
