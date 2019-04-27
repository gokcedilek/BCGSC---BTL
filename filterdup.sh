#!/bin/bash
#read the input file: $1
#write the unique ID and sequences into the output file: $2

#IFS=$'\n'
#for line in $(cat $1)
#do #the awk command finds the unique entries!!!
#$(echo $line | awk 'seen[$0]++') > $2 #overrite the previous contents of the file, if there is any
#done

#awk '{seen[$0]++};END{for(i in seen) if(seen[i]==1)print i}' $1  > $2
#awk '{seen[$0]++};END{print(seen[i]==1)}' $1 > $2
#if the line doesnt start with, check for uniqueness, if unique, direct that line plus one line before to $2

#$(grep -B 1 $(awk '!/^>/{seen[$0]++};END{for(i in seen) if(seen[i]==1)print i}' $1) $1 > $2
#(grep -f <(awk '!/^>/' $1 | awk '{seen[$0]++};END{for(i in seen) if(seen[i]==1) print i}' $1) $1) > $2
#(grep -f <(awk '!/^>/' $1) $1) > $2
#(grep "ID" $1) > $2
#(grep -f <(awk '{seen[$0]++};END{for(i in seen) if(seen[i]==1)print i}' $1) $1) > $2
#unique= $(awk '{seen[$0]++};END{for(i in seen) if(seen[i]==1)print i}' $1)
#grep $pattern $1 > $2
#grep -f <(awk '{seen[$0]++};END{for(i in seen) if(seen[i]==1)print i}' $1) $1

#awk '!/ID/' $1 #works
#awk '{seen[$0]++};END{for(i in seen) if(seen[i]==1)print i}' $1 #works
#awk '!/ID/' $1 | awk '{seen[$0]++};END{for(i in seen) if(seen[i]==1)print i}' #this line gives you all unique sequences
#we need to go one line back (with grep) and also include the ids of each sequence
#then we only should keep one copy of the id
#awk '!/ID/' $1 | awk '{seen[$0]++};END{for(i in seen) if(seen[i]==1)print i}' 
if [ -s "$2" ]; then #if document is not empty 
	echo "The output file is not empty!"
	echo "Do you want to clear the output file? [yes,no]"
	read boolean
	if [ "$boolean" == "yes" ]; then
		echo "Clearing the file!"
		> $2
	elif [ "$boolean" == "no" ]; then
		echo "Not clearing the file... Exiting..."
		exit 1
	else
		echo "Invalid input... Exiting..."
	fi
fi
#maybe set the IFS to be ">"
#for line in $(cat $1):
#do
#	#echo $line
#	id=$(echo $line | awk -F "\n" '{print $1}')
#	seq=$( echo $line | awk -F "\n" '{print $2}')
#	#id= echo $line | awk -F ">" '{print $2}' | awk -F "\n" '{print $1}'| awk 'BEGIN{OFS="\n"}{$1=$1;print $0}'
#	#seq=echo $line | awk -F ">" '{print $2}' | awk -F "\n" '{print $2}'| awk 'BEGIN{OFS="\n"}{$1=$1;print $0}'
#	echo "$id" 
#	echo "$seq" 
#	#grep -B 1 $(awk '!a[seq]++' $1) >> $2
#	#$(awk '!a[seq]++' $1) >> $2
#	#echo ">$id" >> $2
#	#echo "$seq" >> $2
#	#done

#cat $1 | while read -r id; do echo $id >> $2; read seq; echo $seq >> $2; done
#cat $1 | while read id; do read seq; if [[! grep -Fxq "${seq}" $2 ]]; then echo $id >> $2 echo $seq >> $2 fi done 
#IFS=$'\n'
#unique_hits="${1}.uniquehits"
while read -r id; #to read multiple lines separately from a file
do
	read seq; #read the first line as $id , and the second line as $seq
	if ! grep -Fxq "${seq}" $2 #if the output file doesn't contain this sequence
	then
		echo $id >> $2;
		echo $seq >> $2;
	fi
done < $1 #read command needs the input file $1: this symbol < directs the file to the command!
#in this script, we filter the duplicates using SEQUENCES (not IDs) because imagining the same protein sequence with i) same sequence IDs ii) different sequence IDs we want to only keep the "protein sequence" under hand. at this stage, we don't care about uniquely identifying each protein using their id's, we only want all the unique sequences.



