#!/bin/bash

#this script formats the fasta file downloaded for the honeybee amp project
#Note: requires passing the desired name of the file that was downloaded

file_path="/home/gdilek/Downloads"
project_path="/projects/amp/honeybee/gdilek"
old_file_name="sequence.txt"
new_file_name=$1

#when stating commands: variable="$()"
#when stating non-command things (such as the path name): variable=" "
#it is a good idea to put in quotes to prevent confusion (add the dollar sign inside of quotes)

#don't forget the if format; you need [,],then,fi!
#because we have /, we separate the variable names with {} to prevent confusion
if [[ ! -e "${file_path}/${old_file_name}" ]] #if sequence.txt doesn't exist in the home directory, don't keep looking for it for a long time, exit the script
#if [[ ! -e "{$file_path}/{$old_file_name}" ]] #this DOES NOT work!!!
then
	echo "${old_file_name} does not exist!"
	exit 1
fi

#move from downloads to project path
mv "${file_path}/${old_file_name}" "$project_path" #each name could be enclosed by " "

#rename the file in the project path
cd "$project_path"
mv "$old_file_name" "$new_file_name" #dollar sign will be inside the quotes because that as a whole defines the file name

#copy the file before putting everything to one line, in case something goes wrong
cp "$new_file_name" "$new_file_name.copy"

#put the original file to one line
seqtk seq $new_file_name > $new_file_name.singleline; mv $new_file_name.singleline $new_file_name 

#count the number of lines that begin with ">"in both the original file and the copy, if they are the same, remove copy; else, don't do anything because something went wrong, print an error message
num_id_original=$(grep -c '^>' $new_file_name) #since this is a command, we need $()
num_id_copy=$(grep -c '^>' $new_file_name.copy)
if [[ $num_id_original -eq $num_id_copy ]] #comparing numbers: man test --> -eq {check test} #comparing strings: ==
then
	rm $new_file_name.copy
	echo "Successfully moved the sequences to one line, removing copy!"
else
	echo "Couldn't move the sequences to one line, exiting the script!"
fi
