#!/bin/bash
# FILENAME:  cluster_run.sh

#SBATCH -A ljgomez

#SBATCH --nodes=1
#SBATCH --cpus-per-task=20
#SBATCH --time=00:5:00
#SBATCH --export=ALL
#SBATCH --job-name 'ECE_563_HW2'
#SBATCH --output='slurm-%A.out'
#SBATCH --error='slurm-%A.out'
export OMP_NUM_THREADS=20

module load intel
module load gcc


gcc -fopenmp code_B.c -o code_B
echo "Code Compilation Successful"
./code_B

wait
echo "Job Completed"
