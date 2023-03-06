#!/bin/bash
# FILENAME:  cluster_run.sh

#SBATCH -A ljgomez

#SBATCH --nodes=1
#SBATCH --cpus-per-task=20
#SBATCH --time=00:5:00
#SBATCH --export=ALL
#SBATCH --job-name 'ECE_563_HW4'
#SBATCH --output='output_part_1.txt'
#SBATCH --error='output_part_1.txt'


module load intel

icc -qopenmp hw4Loop.c -o hw4Loop
echo "Code Compilation Successful"
./hw4Loop
wait


echo "Job Completed"
