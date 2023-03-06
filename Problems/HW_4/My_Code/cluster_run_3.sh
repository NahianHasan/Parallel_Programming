#!/bin/bash
# FILENAME:  cluster_run.sh

#SBATCH -A ljgomez

#SBATCH --nodes=1
#SBATCH --cpus-per-task=4
#SBATCH --time=00:5:00
#SBATCH --export=ALL
#SBATCH --job-name 'ECE_563_HW4'
#SBATCH --output='output_part_3.txt'
#SBATCH --error='output_part_3.txt'


module load intel

icc -qopenmp hw4Section.c -o hw4Section
echo "Code Compilation Successful"
./hw4Section
wait


echo "Job Completed"
