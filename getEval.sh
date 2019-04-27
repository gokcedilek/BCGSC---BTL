#!/bin/bash

#for every sequence in 20subjAMPs (in the main project dirctory):
jhmmer=$1 #input file to search
#for ID in $(cat 20subjAMPs.querymellifera.blastp | awk -F "|" '{print $2}' )
#for ID in $(cat 20subjAMPs.querymellifera.blastp)
#do
#	#grep this ID from the Jackhmmer output IF the line doesn't contain ">>"
#	#print the second field of the line (E-value)
#	#grep -F "${ID}" $jhmmer | grep -v ">>" | awk '{print $1}'  
#	grep -v ">>" $jhmmer | grep -F "${ID}" | awk '{print $1}'
#done
#grep -v ">>" $jhmmer | grep -F "NC_037641.1_prot_NP_001011607.1_1265" 
for ID in $(cat 20subjAMPs.querymellifera.blastp) #the reference AMPs
do
	#grep -F "${ID}" $jhmmer | awk '!/^>>/' | sed 's/^+//' | awk '{print $9}' #print the subject ID
	#grep -F "${ID}" $jhmmer | awk '!/^>>/' | sed 's/^+//' | awk '{print $1}' | awk -F "-" '{print $2}'
	#echo "${ID}"
	grep -F "${ID}" $jhmmer | awk '!/^>>/'| awk '!/Query:/' | sed 's/^+//' | awk '{print $1}' | awk -F "-" '{print $2}' | awk '{a[i++]=$1} END { for (i in a) { if (a[i] > max) {max=a[i]}} print max}'
#	grep -F "${ID}" $jhmmer | awk '!/^>>/' | awk '!/Query:/'| sed 's/^+//' | awk '{print $1}' | awk -F "-" '{print $2}' | awk '{a[i++]=$1} END { min=a[0]; for (i in a) { if (a[i] <min) {min=a[i]}} print min}'
	#pick the max expo first, which is the min E value. If any AMPs are lost, increase E by decreasing the expo!
	#we take the max expo "for each subject", and then we print the max for each of them
done

