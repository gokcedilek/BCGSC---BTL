#!/bin/bash
#Jackhmmer must be run in from the main file directory because many scripts depend on files whose paths were specified with the main directory being the current directory
#as well, this script must be called from the main file directory!


#1:Jhmmer after check:find the file in the current directory that starts with jh_N--> this specifies the output file! obtain the basename, and pass to the script with its parameters
#one Jhmmer output file at a time!!!
#file=$(ls jh_N*) #in the current directory=main project directory
file=$1
f_name=$(basename $file)
#name structure: jh_N{N_val}_E{E_val}.out
N_val=$(echo $f_name | awk -F "_" '{print $2}'| cut -c 2-)
E_val=$(echo $f_name | awk -F "_" '{print $3}'| awk -F "." '{print $1}'| cut -c 2-)
./scripts/after_jhmmer.sh $file $N_val $E_val NCBImellifera.use > output.txt


#2:make sure all queries converged, also make sure that 18 input AMPs are included!
conv=$(head -n 1 output.txt) #first line 
#convexit=$?
foundAMP=$(tail -n 2 output.txt | head -n 1) #second line 
#foundexit=$?
num=$(tail -n 1 output.txt) #third line
#if not everything converged, or we couldn't find 18 AMPs, print to output; otherwise, porcess as usual
if [[ $conv != "All query AMPs have converged!" ]]
then
	echo $conv
	echo "Exiting"
	exit 1
else
	echo $conv
fi
if ! grep -q "20/20" <(echo $num)
then
	echo $num
	echo "Exiting"
	exit 1
else
	echo $foundAMP
	echo $num
fi
rm output.txt


#everything worked as expected up to here; ie. we had all queries converged, and we hit 18 of the input a.mellifera AMPs!!!
#3: check for the "exact" presence of 18 input a.mellifera AMPs from BLASTP:
#cat queryAMPsinHits_N${N_val}_E${E_val} | awk -F ">" '{print $2}' | awk '{print $1}' > ${E_val}.id
#./scripts/compare2.sh ${E_val}.id 18queryAMPs.querymellifera.blastp > output.txt
#if [[ $(wc -l < output.txt) -ne 2 ]]
#then
#	cat output.txt
#	echo "Exiting"
#	exit 1
#else
#	cat output.txt
#fi
#rm output.txt

#4: check for the presence of 20 apis mellifera AMPs that must be hit with Jackhmmer:
./scripts/extractIDname.sh hits_N${N_val}_E${E_val} > E${E_val}.IDcheck
./scripts/showDiff.sh 20subjAMPs.querymellifera.blastp E${E_val}.IDcheck > output.txt
if [[ $(wc -l < output.txt) -ne 1 ]]
then
	cat output.txt
	echo "Exiting"
	exit 1
else
	cat output.txt
fi
rm output.txt



   






