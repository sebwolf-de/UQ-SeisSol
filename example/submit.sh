#!/bin/bash
#SBATCH -J UQ-SeisSol
#SBATCH -o ./%x.%j.%N.out
#SBATCH -D ./
#SBATCH --get-user-env
#SBATCH --clusters=cm2
#SBATCH --partition=cm2_std
#SBATCH --qos=cm2_std
#SBATCH --nodes=24
#SBATCH --ntasks-per-node=1
#SBATCH --mail-type=end
#SBATCH --mail-user=ADD YOUR EMAIL HERE
#SBATCH --export=NONE
#SBATCH --time=00:20:00
  
module load slurm_setup
unset KMP_AFFINITY
export OMP_NUM_THREADS=54
export OMP_PLACES="cores(27)"

#clean output dir just to be sure
rm -rf output/*
#run UQ
./main uq.yaml
