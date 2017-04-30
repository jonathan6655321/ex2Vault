#!/bin/bash

gcc -o vault *.c -Werror

echo Result: A vault created
./vault my_repository.vlt INit 2M

echo Result: file_name inserted
./vault my_repository.vlt aDd folder1/folder2/data_filter10.c
./vault my_repository.vlt Add folder1/folder2/data_filter11.c
./vault my_repository.vlt adD folder1/folder2/data_filter12.c
./vault my_repository.vlt add folder1/folder2/data_filter13.c

echo my_file1.txt 3B 0755 Thurs Apr 06 21:18:23 2017
./vault my_repository.vlt liSTt

echo Result: file_name deleted
./vault my_repository.vlt rm data_filter12.c

echo Result: some_file_name created
./vault my_repository.vlt fetch data_filter13.c
rm data_filter13.c

echo Result: Defragmentation complete
./vault my_repository.vlt defrag


echo Number of files: 2
echo Total size: 6023B
echo Fragmentation ratio: 0.0

./vault my_repository.vlt status

