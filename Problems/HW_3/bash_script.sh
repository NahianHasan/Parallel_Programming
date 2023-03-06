#!/bin/bash

echo "hw4a.c"
gcc -fopenmp hw4a.c -o hw4a
echo "Code Compilation Successful"
./hw4a

echo ""

echo "hw4b"
gcc -fopenmp hw4b.c -o hw4b
echo "Code Compilation Successful"
./hw4b

echo ""

echo "hw4c"
gcc -fopenmp hw4c.c -o hw4c
echo "Code Compilation Successful"
./hw4c

gcc -fopenmp part_b.c -o part_b
echo "Code Compilation Successful"
./part_b

gcc -fopenmp part_c.c -o part_c
echo "Code Compilation Successful"
./part_c

gcc -fopenmp part_d.c -o part_d
echo "Code Compilation Successful"
./part_d
