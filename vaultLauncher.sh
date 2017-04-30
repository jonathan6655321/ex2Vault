#!/bin/bash

gcc -o vault *c

#for i in `seq 1 100`;
#do

#	rm folder1/folder2/data_filter"$i".c
#	cp folder1/folder2/data_filter.c folder1/folder2/data_filter"$i".c
#done  

./vault my_repository.vlt init 10M 

./vault my_repository.vlt add folder1/folder2/data_filter.c

./vault my_repository.vlt status 



./vault my_repository.vlt add folder1/folder2/data_filter1.c 

./vault my_repository.vlt status 



./vault my_repository.vlt add folder1/folder2/data_filter2.c 

./vault my_repository.vlt status 


./vault my_repository.vlt rm data_filter1.c

./vault my_repository.vlt status 


./vault my_repository.vlt add folder1/folder2/a2MegaFile


./vault my_repository.vlt status 
 
./vault my_repository.vlt defrag 
 
./vault my_repository.vlt status 

#./vault my_repository.vlt add folder1/folder2/data_filter3.c 
#./vault my_repository.vlt add folder1/folder2/data_filter4.c 
#./vault my_repository.vlt add folder1/folder2/data_filter5.c 
