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

# Rebuild the code
cd build
make
ls | grep job | xargs chmod +x

# Now submit the batch job
COMMIT_HASH=$(git rev-parse --short HEAD) # get the short commit hash
TIMESTAMP=$(date +%Y%m%d%H%M%S)
sbatch \
	--output=../log/${TIMESTAMP}_${COMMIT_HASH}_%j_job-speedup.o \
	--error=../log/${TIMESTAMP}_${COMMIT_HASH}_%j_job-speedup.e \
	job-speedup
sbatch \
	--output=../log/${TIMESTAMP}_${COMMIT_HASH}_%j_job-l2.o \
	--error=../log/${TIMESTAMP}_${COMMIT_HASH}_%j_job-l2.e \
	job-l2
sbatch \
	--output=../log/${TIMESTAMP}_${COMMIT_HASH}_%j_job-l3.o \
	--error=../log/${TIMESTAMP}_${COMMIT_HASH}_%j_job-l3.e \
	job-l3
sbatch \
	--output=../log/${TIMESTAMP}_${COMMIT_HASH}_%j_job-flops-dp.o \
	--error=../log/${TIMESTAMP}_${COMMIT_HASH}_%j_job-flops-dp.e \
	job-flops-dp
