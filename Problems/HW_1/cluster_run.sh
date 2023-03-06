#!/bin/bash
# FILENAME:  cluster_run.sh

#SBATCH -A ljgomez

#SBATCH --nodes=1
#SBATCH --cpus-per-task=20
#SBATCH --time=00:01:00
#SBATCH --export=ALL
#SBATCH --job-name 'ECE_56300'
#SBATCH --output='slurm-%A.out'
#SBATCH --error='slurm-%A.out'


module load intel
icc -qopenmp omp_hello.c -o omp_hello
#module load gcc
#gcc -fopenmp myprogram.c -o myprogram

export OMP_NUM_THREADS=20
./omp_hello
wait
echo "Job Completed"
