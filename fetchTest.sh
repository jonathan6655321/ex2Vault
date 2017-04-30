#!/bin/bash

gcc -o vault  *.c

./vault v.vlt init 2M

var=1

for i in `seq 1 100`
do


	./vault v.vlt add folder1/folder2/data_filter.c
	
	./vault v.vlt add folder1/folder2/data_filter$i.c
	./vault v.vlt fetch data_filter.c
		
	
	result=$(diff -y -W 72 data_filter.c folder1/folder2/data_filter.c)

if [ $? -eq 0 ]
then
        echo "OK: passed fetchFile.c test"
else
        echo "FAILED!!! fetchFile.c test"
        echo "$result"
	var=0
fi


	rm data_filter.c
	./vault v.vlt rm data_filter.c
	
	./vault v.vlt defrag
done


if [ "$var" ==  0 ]
then
	echo FAILED
else
	echo PASSED
fi


