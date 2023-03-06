#!/bin/bash
# FILENAME:  cluster_run_Q1.sh

#SBATCH -A ljgomez

#SBATCH --nodes=1
#SBATCH --cpus-per-task=20
#SBATCH --time=00:5:00
#SBATCH --export=ALL
#SBATCH --job-name 'ECE_563_THE1'
#SBATCH --output='Q1.output'
#SBATCH --error='Q1.output'
export OMP_NUM_THREADS=4

module load intel
module load gcc


gcc -fopenmp Q1.c -o Q1
#echo "Code Compilation Successful"
./Q1

wait
#echo "Job Completed"
