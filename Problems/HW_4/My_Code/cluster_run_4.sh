#!/bin/bash
# FILENAME:  cluster_run.sh

#SBATCH -A ljgomez

#SBATCH --nodes=1
#SBATCH --cpus-per-task=16
#SBATCH --time=00:5:00
#SBATCH --export=ALL
#SBATCH --job-name 'ECE_563_HW4'
#SBATCH --output='output_part_4.txt'
#SBATCH --error='output_part_4.txt'


module load intel

icc -qopenmp hw4Q.c -o hw4Q
echo "Code Compilation Successful"
./hw4Q
wait


echo "Job Completed"
