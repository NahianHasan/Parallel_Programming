#!/bin/bash

#gcc -fopenmp serial_word_count.c -o serial_word_count
#./serial_word_count

gcc -fopenmp omp_word_count.c -o omp_word_count
./omp_word_count
