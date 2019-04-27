#!/bin/bash
IFS=$'\n'

#for every transcriptome sequence, identify if it has a poly A tail
#input: the sample (folder) name in the transcriptome_assembly directory, a cut-off value for number of A's we want to investigate
#output: sequences that don't have a poly-A tail as long as the cut-off value

#path="/projects/amp/honeybee/gdilek/transcriptome_assembly"
#RNA_seq="$path/$1/rnabloom.transcripts.fa"
cut_off=$2
string=$(for i in $(seq 1 $cut_off); do echo -n "A"; done)
RNA_seq=$1
while read id;
do
	read seq;
	#echo "$(echo $seq | tail -c $cut_off)"
	if [ "$(echo $seq | tail -c $cut_off)" != "$string" ]
	then
	   #there is a sequence without poly-A tail!
	   echo "$id"
	   echo "$seq"
	fi
done < $RNA_seq


#for i in $(seq 1 $cut_off); do echo -n "A"; done
#for i in $(eval echo '{1.."$cut_off"}'); do echo -n "A"; done
#printf 'A%.0s' $(eval echo '{1..$cut_off}')
