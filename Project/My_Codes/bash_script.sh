#!/bin/bash
# FILENAME:  cluster_run.sh

#SBATCH -A scholar

#SBATCH --nodes=1
#SBATCH --cpus-per-task=20
#SBATCH --time=00:5:00
#SBATCH --export=ALL
#SBATCH --job-name 'ECE_563_Proj'
#SBATCH --output='output_omp.txt'
#SBATCH --error='output_omp.txt'


#module load intel
#module load gcc
#icc -qopenmp omp_word_count.c -o omp_word_count
#gcc -fopenmp serial_word_count_2.c -o serial_word_count_2
#./serial_word_count_2

#gcc -fopenmp omp_word_count.c -o omp_word_count
#./omp_word_count

gcc -fopenmp mpi_word_count.c -o mpi_word_count
./mpi_word_count
wait


echo "Job Completed"
