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


./vault my_repository.vlt init 2M
res=$(./vault my_repository.vlt add folder1/folder2/a2MegaFile)
if [ "$res" == "Error: not enough space for file in vault" ]; then
	echo OK: addFile.c  not enough space for file in vault
else
	echo FAIL!!! addFile.c not enough space for file in vault
fi




./vault my_repository.vlt init 510K
var=0
for i in `seq 1 10`
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



echo ======= tests for listFiles.c ======


./vault my_repository.vlt list



echo ======= tests for removeFile.c ======


var=0
for i in `seq 1 10`
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

echo ========================================
echo ============ TESTS DONE: ===============
echo ========================================
