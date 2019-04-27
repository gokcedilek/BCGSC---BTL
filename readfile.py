#!/usr/bin/env python3
import sys
with open(sys.argv[1]) as f1:
	lines1= f1.readlines()
with open(sys.argv[2]) as f2:
    lines2= f2.readlines()
#until you are not at the end of the file, for each line in file1, check if it is contained in file2, if not copy 
for i in range (0, len(lines1),2):
	the_id=lines1[i]
	seq=lines1[i+1]
	#print(the_id+ seq)
	if seq not in lines2:
		#print( the_id + "\n" + seq)
		#print "%s" % (seq)
		print(the_id+ seq.rstrip()) #Diana showed that rstrip() removes the newline!






