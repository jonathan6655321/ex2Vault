#!/bin/bash

gcc -o vault *c

echo ========================================
echo ============ TEST RESULTS: =============
echo ========================================

echo ======= tests for vault.c     ======
res=$(./vault my_repository.vlt gibrishOP a b s)
if [ "$res" == "Error: invalid operation argument" ]; then
	echo OK: vault.c invalid operation test
else
	echo FAIL!!! vailt.c invalid operation test
fi


res=$(./vault 1)
if [ "$res" == "Error: invalid number of arguments" ]; then
	echo OK: vault.c invalid number of arguments test
else
	echo FAIL!!! vailt.c invalid number of arguments test
fi

echo ======= tests for initVault.c ======
res=$(./vault my_repository.vlt init)
if [ "$res" == "Error: invalid number of arguments for init" ]; then
	echo OK: initVault.c invalid number of arguments for init
else
	echo FAIL!!! initVault.c invalid number of arguments for init
fi

#one 'ENTER' per \n in original text
res=$(./vault my_repository.vlt init 2F)
if [ "$res" == "Error: invalid vault size argument

Error: couldn't initialize the repoMetaData" ]; then
	echo OK: initVault.c invalid vault size format
else
	echo FAIL!!! initVault.c invalid vault size format
fi 



res=$(./vault my_repository.vlt init 32031B)
if [ "$res" == "Error: repository-total-size request not large enough" ]; then
	echo OK: initVault.c repository-total-size request not large enough
else
	echo FAIL!!! initVault.c repository-total-size request not large enough
fi

./vault my_repository.vlt init 2G

FILESIZE=$(stat -c%s "my_repository.vlt")
if [ "$FILESIZE" == "2147483649" ]; then
	echo OK: initVault.c file size fits request
else
	echo FAIL!!! initVault.c file size doesnt fit request
fi


echo ======= tests for addFile.c ======


./vault my_repository.vlt init 4M
res=$(./vault my_repository.vlt add folder1/folder2/a2MegaFile)
if [ "$res" == "Error: not enough space for file in vault" ]; then
	echo OK: addFile.c  not enough space for file in vault
else
	echo FAIL!!! addFile.c not enough space for file in vault
fi




./vault my_repository.vlt init 5M
var=0
for i in `seq 1 100`
do
	
	res=$(./vault my_repository.vlt add folder1/folder2/data_filter"$i".c)
	if [ "$res" != "Result: data_filter$i.c inserted" ]; then
		echo FAIL!!! on addFile.c , file $i
		var=1		 
#	else 
#		echo OK: on addFile.c , file $i
	fi

done

if [ "$var" == "1" ]; then
	echo FAILED add.c TEST
else
	echo OK: addFile.c passed for 10 file insert
fi



echo ======= tests for listFiles.c ======


./vault my_repository.vlt list

echo ======= tests for fetchFile.c ======


./vault my_repository.vlt fetch data_filter10.c

result=$(diff -y -W 72 data_filter10.c folder1/folder2/data_filter10.c)

if [ $? -eq 0 ]
then
        echo "OK: passed fetchFile.c test"
else
        echo "FAILED!!! fetchFile.c test"
        echo "$result"
fi

rm data_filter10.c
./vault my_repository.vlt fetch data_filter3.c

result=$(diff -y -W 72 data_filter3.c folder1/folder2/data_filter3.c)

if [ $? -eq 0 ]
then
        echo "OK: passed fetchFile.c test"
else
        echo "FAILED!!! fetchFile.c test"
        echo "$result"
fi


rm data_filter3.c

echo ======= tests for removeFile.c ======


var=0
for i in `seq 1 100`
do
	
#./vault my_repository.vlt list
	res=$(./vault my_repository.vlt rm data_filter"$i".c)
	if [ "$res" != "Result: data_filter"$i".c deleted" ]; then
		echo $res
		echo FAIL!!! on removeFile.c , file $i
		var=1		 
	fi
	
done

./vault my_repository.vlt list


if [ "$var" == "1" ]; then
	echo FAILED removeFile.c TEST
else
	echo OK: passed removeFile.c Test
fi



echo ======= tests for getFileStatus.c ======



./vault my_repository.vlt init 5M
var=0
for i in `seq 1 100`
do
	
	res=$(./vault my_repository.vlt add folder1/folder2/data_filter"$i".c)
	if [ "$res" != "Result: data_filter$i.c inserted" ]; then
		echo FAIL!!! on addFile.c , file $i
		var=1		 
	fi
	
done

if [ "$var" == "1" ]; then
	echo FAILED add.c TEST
else
	echo OK: addFile.c passed for 100 file insert
fi

./vault my_repository.vlt status
./vault my_repository.vlt rm data_filter5.c

./vault my_repository.vlt status
./vault my_repository.vlt rm data_filter1.c
./vault my_repository.vlt list

./vault my_repository.vlt status




echo ======= tests for defragVault.c ======

./vault my_repository.vlt defrag

./vault my_repository.vlt status


echo ============= another fetch after the defrag ===========
./vault my_repository.vlt fetch data_filter3.c

result=$(diff -y -W 72 data_filter3.c folder1/folder2/data_filter3.c)

if [ $? -eq 0 ]
then
        echo "OK: passed fetchFile.c test"
else
        echo "FAILED!!! fetchFile.c test"
fi

rm data_filter3.c


./vault my_repository.vlt fetch data_filter27.c

result=$(diff -y -W 72 data_filter27.c folder1/folder2/data_filter27.c)

if [ $? -eq 0 ]
then
        echo "OK: passed fetchFile.c test"
else
        echo "FAILED!!! fetchFile.c test"
fi

rm data_filter27.c


echo ========================================
echo ============ TESTS DONE: ===============
echo ========================================
