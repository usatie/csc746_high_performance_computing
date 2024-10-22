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
sbatch \
	--output=../log/job-blas.o.${COMMIT_HASH}.%j \
	--error=../log/job-blas.e.${COMMIT_HASH}.%j \
	job-blas
sbatch \
	--output=../log/job-basic-omp.o.${COMMIT_HASH}.%j \
	--error=../log/job-basic-omp.e.${COMMIT_HASH}.%j \
	job-basic-omp
sbatch \
	--output=../log/job-blocked-omp.o.${COMMIT_HASH}.%j \
	--error=../log/job-blocked-omp.e.${COMMIT_HASH}.%j \
	job-blocked-omp
sbatch \
	--output=../log/job-l2.o.${COMMIT_HASH}.%j \
	--error=../log/job-l2.e.${COMMIT_HASH}.%j \
	job-l2
sbatch \
	--output=../log/job-l3.o.${COMMIT_HASH}.%j \
	--error=../log/job-l3.e.${COMMIT_HASH}.%j \
	job-l3
