#!/bin/bash

gcc -o vault  *.c

./vault v.vlt init 4M

	rm a2MegaFile

var=1

for i in `seq 1 6`
do

	
	./vault v.vlt add folder1/folder2/data_filter$i.c
		
done

	./vault v.vlt rm data_filter1.c

	./vault v.vlt rm data_filter3.c

	./vault v.vlt rm data_filter5.c

	./vault v.vlt add folder1/folder2/a2MegaFile


	./vault v.vlt fetch a2MegaFile
	result=$(diff -y -W 72 a2MegaFile folder1/folder2/a2MegaFile)

if [ $? -eq 0 ]
then
        echo "OK: passed fetchFile.c test"
else
        echo "FAILED!!! fetchFile.c test"
        echo "$result"
	var=0
fi


#	rm a2MegaFile
#	./vault v.vlt rm a2MegaFile
	
#	./vault v.vlt defrag


./vault v.vlt list

if [ "$var" ==  0 ]
then
	echo FAILED
else
	echo PASSED
fi


