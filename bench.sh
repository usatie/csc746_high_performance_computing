#!/bin/bash -l

cd build

# Prepare user env needed for Slurm batch job
# such as module load, setup runtime environment variables, or copy input files, etc.
# Basically, these are the commands you usually run ahead of the srun command
module load gpu
module load e4s/23.05
spack env activate gcc
spack load likwid
unset LUA_PATH
unset LUA_CPATH

export SBATCH_ACCOUNT=m3930
export SALLOC_ACCOUNT=m3930

# Generate the Slurm batch script below with the here document,
# then when sbatch the script later, the user env set up above will run on the login node
# instead of on a head compute node (if included in the Slurm batch script),
# and inherited into the batch job.
COMMIT_HASH=$(git rev-parse --short HEAD) # get the short commit hash
cat << 'EOF' > job-benchmark.sh
#!/bin/bash -l
#SBATCH --constraint=cpu
#SBATCH --cpus-per-task=1
#SBATCH --qos=debug
#SBATCH --nodes=1
#SBATCH --account=m3930
#SBATCH --perf=generic
#SBATCH --time=00:05:00
#SBATCH --job-name=job-benchmark
#SBATCH --output=job-benchmark.o.${COMMIT_HASH}.%j
#SBATCH --error=job-benchmark.e.${COMMIT_HASH}.%j

./job-blas
./job-basic-omp
./job-blocked-omp
EOF

# Now submit the batch job
sbatch job-benchmark.sh
