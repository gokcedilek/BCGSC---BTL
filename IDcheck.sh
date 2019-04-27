#!/bin/bash
if [[ "$#" -ne 1 ]]
then
	echo "USAGE: $(basename $0) <Jackhmmer hits directory/file>" 1>&2
	exit 1
fi
jhdir=$(echo $1 | awk -F "/" '{print $1}')
jhfile=$(echo $1 | awk -F "/" '{print $2}')
#echo "${jhdir}"
#echo "${jhfile}"
cd $jhdir
#cd jhmmer_mellifera/
#for file in $(ls hits_*)
#do
#	E_name=$(basename $file | awk -F "_" '{print $3}')
#	#echo $E_name
#	../scripts/extractIDname.sh $file > ${E_name}.IDcheck
#	../scripts/showDiff.sh ../20subjAMPs.querymellifera.blastp ${E_name}.IDcheck
#done

#Not automated version:
E_name=$(basename $jhfile | awk -F "_" '{print $3}')
../scripts/extractIDname.sh $jhfile > ${E_name}.IDcheck #extract the ID from JH hits
../scripts/showDiff.sh ../20subjAMPs.querymellifera.blastp ${E_name}.IDcheck #check if hit IDs contain all of the 20 subj AMP IDs


