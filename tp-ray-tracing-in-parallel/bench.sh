#!/bin/bash -l

# Prepare user env needed for Slurm batch job
# such as module load, setup runtime environment variables, or copy input files, etc.
# Basically, these are the commands you usually run ahead of the srun command
module load gpu
module load e4s/23.08
spack env activate gcc
spack load likwid
unset LUA_PATH
unset LUA_CPATH

export SBATCH_ACCOUNT=m3930
export SALLOC_ACCOUNT=m3930

sbatch job-rt
