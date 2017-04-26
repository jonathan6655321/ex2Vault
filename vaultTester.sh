#!/bin/bash

gcc -o vault *c

declare -a arr=("init" "list" "add" "rm" "fetch" "defrag" "status" "gibrish")

for i in "${arr[@]}"  
do
./vault my_repository.vlt "$i"
done

# testing initVault:
./vault my_repository.vlt init 2M


