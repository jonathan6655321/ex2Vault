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

echo ========================================
echo ============ TESTS DONE: ===============
echo ========================================

#declare -a arr=("init" "list" "add" "rm" "fetch" "defrag" "status" "gibrish")

#for i in "${arr[@]}"  
#do
#./vault my_repository.vlt "$i"
#done

# testing initVault:
#./vault 

