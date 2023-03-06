#!/bin/bash

echo "Code A"
gcc -fopenmp code_A.c -o code_A
echo "Code Compilation Successful"
./code_A

echo ""

echo "Code B"
gcc -fopenmp code_B.c -o code_B
echo "Code Compilation Successful"
./code_B

echo ""

echo "Code C"
gcc -fopenmp code_C.c -o code_C
echo "Code Compilation Successful"
./code_C

echo ""

echo "slow.c"
g++ -fopenmp slow.c -o slow
echo "Code Compilation Successful"
./slow

echo ""

echo "verySlow.c"
g++ -fopenmp verySlow.c -o verySlow
echo "Code Compilation Successful"
./slow
