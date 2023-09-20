#!/bin/bash
#SBATCH -J UQ-SeisSol
#SBATCH -o ./logs/%x.%j.out
#SBATCH -D ./
#SBATCH --get-user-env
#SBATCH --clusters=cm2
#SBATCH --partition=cm2_large
#SBATCH --qos=cm2_large
#SBATCH --nodes=32
#SBATCH --ntasks-per-node=1
#SBATCH --mail-type=end
#SBATCH --mail-user=wolf.sebastian@cit.tum.de
#SBATCH --export=NONE
#SBATCH --time=24:00:00
  
module load slurm_setup
unset KMP_AFFINITY
export OMP_NUM_THREADS=54
export OMP_PLACES="cores(27)"

#clean output dir just to be sure
mkdir -p output/chain
mkdir -p output/current
rm -rf output/chain/*
rm -rf output/current/*
rm SeisSol_stderr.txt
rm SeisSol_stdout.txt
#run UQ
./main uq.yaml
cp test.h5 test-8.h5
