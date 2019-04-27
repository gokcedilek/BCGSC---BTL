#!/bin/bash

#if [[ "$#" -ne 3 ]]
#then
#	echo "USAGE: $(basename $0) <Jackhmmer output file N-value E/T-value>" 1>&2
#	exit 1
#fi
#
#given the Jhmmer output file
jhmmer=$1

#echo "Is the Jackhmmer run with E-value or bit-score (E,T)?" 1>&2 #redirect to STDERR, so that this WON'T be displayed in the standard output!
#read ans #read also works for standard error
#if [[ $ans == 'E' ]]
if [[ $(echo $(basename $jhmmer) | awk -F "_" '{print $3}'| cut -c1) == 'E' ]]
then
	hits_filename="hits_N${2}_E${3}"
else
	hits_filename="hits_N${2}_T${3}"
fi

num_query=$(grep -c 'Query:' $jhmmer)
num_conv=$(grep -c 'CONVERGED' $jhmmer)
if [[ $num_query -ne $num_conv ]]
then
	echo "There are $(($num_query-$num_conv)) query AMPs that have not converged!"
else
	echo "All query AMPs have converged!"
fi

#obtain the UNIQUE subject hits IDs and sequences(from the reference file) from jhmmer output file, and put to the hits file:
seqtk subseq ./reference/mellifera_prot_oneLine.faa <(awk -F ">> " '/^>>/ {print $2}' $jhmmer | sort -u) > $hits_filename

#check if subject hits contain the input honeybee AMPs!!!
#read sequences from the input honeybee AMPs, and for each AMP, if it is hit with jackhmmer, output it to a hitAMPs file!
inputAMPs=20subjAMPs.querymellifera.blastp #this file is apis m. AMPs from the 20 subject AMPs-to check if all apis m. AMPs from the reference subject sequences are preserved(hit) in the run!!
hitAMPs_filename="hitsubjAMPs_N${2}_E${3}"
#queryAMPs_filename="queryAMPsinHits_N${2}_E${3}"
subjHits=0

if [[ -e "$hitAMPs_filename" ]]
then
	rm $hitAMPs_filename
fi
touch $hitAMPs_filename #we need to create this here to be able to execute if condition for the first time: if [[ "$(grep -c ${seq} $hitAMPs_filename)" -eq 0 ]]

while read id;
do
	if grep -Fq ${id} $hits_filename
	then
		subjHits=$((subjHits+1))
		grep -A 1 --no-group-separator ${id} $hits_filename >> $hitAMPs_filename
	fi
done < $inputAMPs
uniq_hits=$(grep -c ">" $hits_filename)
echo "There are ${uniq_hits} unique Jackhmmer hits!"
echo "There are ${subjHits}/20 reference apis mellifera proteins included in the subject Jackhmmer hits!"
#add the sanity check and compare2's output!







#if [[ -e "$queryAMPs_filename" ]]
#then
#	rm $queryAMPs_filename
#fi
##we are appending to these, so if a previous one exists, delete it and restart from scratch!
#
#queriesInHits=0
#subjHits=0
#while read id;
#do
#	read seq
#	if grep -Fxq ${seq} $hits_filename #if one of the input a.mellifera AMP seqs are found in hits, get the corr subj id and add the id and the seq
#	then
#		if [[ "$(grep -c ${seq} $hitAMPs_filename)" -eq 0 ]]
#		then
#			subjHits=$((subjHits+1))
#			grep -B 1 --no-group-separator ${seq} $hits_filename >> $hitAMPs_filename
#		fi
#		queriesInHits=$((queriesInHits+1))
#		echo "${id}" >> $queryAMPs_filename
#		echo "${seq}" >> $queryAMPs_filename #put the version in NCBI.mellifera.use
#	fi
#done < $inputAMPs
#echo "There are ${queriesInHits} input apis mellifera AMPs included in the subject Jackhmmer hits!"
#uniq_hits=$(grep -c ">" $hits_filename)
#echo "There are ${uniq_hits} unique Jackhmmer hits!"
#echo "There are ${subjHits} reference apis mellifera proteins included in the subject Jackhmmer hits!"

#while read id;
#do
#	read seq #check the sequences when checking for the presence of input AMPs
#	if grep -Fxq ${seq} $hits_filename #this input AMP sequence is contained in jhmmer hits
#	then
#		queriesInHits=$((queriesInHits+1)) #should be 18 at the end of the loop!
#		for i in $(grep -B1 -Fx --no-group-separator "${seq}" $hits_filename | awk '/^>/ {print $1}')
#		do
#			if [[ "$(grep -c "$i" $hitAMPs_filename)" -eq 0 ]]
#			then
##				grep -F -A1 --no-group-separator "$i" $hits_filename
#				grep -F -A1 --no-group-separator "$i" $hits_filename >> $hitAMPs_filename #put the version in mellifera_protein_oneline.faa
#
#			fi
#		done
#		echo "${id}" >> $queryAMPs_filename
#		echo "${seq}" >> $queryAMPs_filename #put the version in NCBI.mellifera.use
#	fi
#done < $inputAMPs
#echo "There are ${queriesInHits} input apis mellifera AMPs included in the subject Jackhmmer hits!"
