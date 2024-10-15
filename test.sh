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
cat << 'EOF' > job-test.sh
#!/bin/bash -l
#SBATCH --constraint=cpu
#SBATCH --cpus-per-task=1
#SBATCH --qos=debug
#SBATCH --nodes=1
#SBATCH --account=m3930
#SBATCH --perf=generic
#SBATCH --time=00:05:00
#SBATCH --job-name=job-test
#SBATCH --output=job-test.o%j
#SBATCH --error=job-test.e%j

# turn off OMP_NUM_THREADS as it doesn't play nicely with likwid-perfctr on Perlmutter
unset OMP_NUM_THREADS

# set some openmp variables:
# OMP_PLACES=threads maps OpenMP threads to hardware threads
# OMP_PROC_BIND=spread binds threads as evenly as possible
#
# see https://docs.nersc.gov/jobs/affinity/ for more information

export OMP_PLACES=threads
export OMP_PROC_BIND=spread

# use PERF_COUNTER_GROUP to tell likwif-perfctr which perf counter group to monitor
# the default here, FLOPS_DP, counts the number of double-precisions FLOPS
export PERF_COUNTER_GROUP=FLOPS_DP

# use the MARKER_FLAG variable to activate LIKWID's marker API, which will result in
# perf counter data being collected only in the region of code surrounded by marker start/stop
# calls. If MARKER_FLAG is not defined, then the LIKWID marker API will not be activated,
# and the resulting performance data reflects the run of the entire program
export MARKER_FLAG="-m"

# iterate over some number of threads. You will need to modify this for HW4 to reflect
# the levels of concurrency in the assignment

# Note: in HW4, we are NOT using OMP_NUM_THREADS to tell your program and OpenMP the
# concurrency level. instead, we tell likwid-perfctr the concurrency level,
# and then likwid-perfctr manages setting OMP_NUM_THREADS

for N in 128 # loop over problem sizes
   do

   # Test basic-omp
   echo "Basic"
   for t in 1 64  # loop over concurrency level
      do
      let maxcore=$t-1

      echo likwid-perfctr $MARKER_FLAG -g $PERF_COUNTER_GROUP -C N:0-$maxcore ./benchmark-basic-omp -N $N
      likwid-perfctr $MARKER_FLAG -g $PERF_COUNTER_GROUP -C N:0-$maxcore ./benchmark-basic-omp -N $N

   done # iterate over concurrency level

   # Test blas
   echo "CBLAS"
   for t in 1  # if running the blas version, uncomment this line and comment out the previous line looping over t in 1 4 16 64
      do
      let maxcore=$t-1

      echo likwid-perfctr $MARKER_FLAG -g $PERF_COUNTER_GROUP -C N:0-$maxcore ./benchmark-blas -N $N
      likwid-perfctr $MARKER_FLAG -g $PERF_COUNTER_GROUP -C N:0-$maxcore ./benchmark-blas -N $N

   done # iterate over concurrency level

   # Test blocked
   echo "BMMCO"
   for B in 4  # uncomment these two lines
      do            # to also iterate over block sizes for the blocked version
      for t in 1 64  # loop over concurrency level
         do
         let maxcore=$t-1

         echo likwid-perfctr $MARKER_FLAG -g $PERF_COUNTER_GROUP -C N:0-$maxcore ./benchmark-blocked-omp -N $N -B $B
         likwid-perfctr $MARKER_FLAG -g $PERF_COUNTER_GROUP -C N:0-$maxcore ./benchmark-blocked-omp -N $N -B $B

      done # iterate over concurrency level
   done # iterate over block size, uncomment me if doing a loop over blocks
done # iterate over problem size

EOF

# Now submit the batch job
sbatch job-test.sh
