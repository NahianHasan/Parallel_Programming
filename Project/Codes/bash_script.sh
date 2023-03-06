#!/bin/bash
# FILENAME:  cluster_run.sh

#SBATCH -A scholar

#SBATCH --nodes=4
#SBATCH --ntasks-per-node=20
#SBATCH --time=00:5:00
#SBATCH --export=ALL
#SBATCH --job-name 'ECE_563_Proj'
#SBATCH --output='output_omp.txt'
#SBATCH --error='output_omp.txt'


#module load intel
module load gcc


#icc -qopenmp omp_word_count.c -o omp_word_count
gcc -fopenmp serial_word_count.c -o serial_word_count
./serial_word_count

gcc -fopenmp omp_word_count.c -o omp_word_count
./omp_word_count

#module load impi
#mpiicc -fopenmp mpi_word_count.c -o mpi_word_count
#srun --mpi=pmi2 -n 48 ./mpi_word_count 

wait


echo "Job Completed"
