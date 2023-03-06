#!/bin/bash
# FILENAME:  cluster_run.sh

#SBATCH -A scholar

#SBATCH --nodes=1
#SBATCH --cpus-per-task=20
#SBATCH --time=00:5:00
#SBATCH --export=ALL
#SBATCH --job-name 'ECE_563_HW5'
#SBATCH --output='cluster_output.txt'
#SBATCH --error='cluster_output.txt'

module load intel

icc -qopenmp main.c -o main
echo "Code Compilation Successful"
./main
wait


echo "Job Completed"
