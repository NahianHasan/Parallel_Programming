#!/bin/bash
# FILENAME:  cluster_run_Q2.sh

#SBATCH -A ljgomez

#SBATCH --nodes=1
#SBATCH --cpus-per-task=20
#SBATCH --time=00:5:00
#SBATCH --export=ALL
#SBATCH --job-name 'ECE_563_THE1'
#SBATCH --output='Q2.output'
#SBATCH --error='Q2.output'
export OMP_NUM_THREADS=4

module load intel
module load gcc


gcc -fopenmp Q2.c -o Q2
#echo "Code Compilation Successful"
./Q2

wait
#echo "Job Completed"
