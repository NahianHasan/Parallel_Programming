#!/bin/bash
# FILENAME:  cluster_run_mpi.sh

#SBATCH -A scholar

#SBATCH --nodes=1
#SBATCH --ntasks-per-node=16
#SBATCH --time=00:5:00
#SBATCH --export=ALL
#SBATCH --job-name 'ECE_563_HW8'
#SBATCH --output='mm.out'
#SBATCH --error='mm.out'


module load openmpi

mpicc mm.c -o mm
echo "Code Compilation Successful"
#Please change the number of threads inside the program main function as well.
srun --mpi=pmi2 -n 16 ./mm

wait
echo "Job Completed"

