#  mpi-2dmesh-harness-instructional

This project contains the code harness for doing an MPI-based scatter-process-gather
motif.

What the code provides:  
* Loads input files, parses command line arguments, writes output  
* Performs domain decomposition using one of three different strategies: row-slab, column-slab, or tiled,   sets up tiles on every rank that contain metadata like tile size, extents, etc.  , assigns tiles to ranks  
* Executes a scattering step by iterating over all tiles and invoking a method to send
data from rank 0 to other ranks, and from non-zero ranks to receive data from rank 0.
* Executes a processing step by iterating over all tiles and sets the stage for user code to be executed on a tile  
* Executes a tile gather step, where data from all tiles not owned by rank 0 is sent back to rank 0  
* Writes output to a disk file  

# Platforms

In principle, this code will build and run on both Perlmutter@NERSC and the VM. It's
fine to use the VM for initial development and testing, but please run your
performance tests on Perlmutter.

# Setting up your NERSC environment

    module load cpu
    export CC=cc
    export CXX=CC

**Note**   
If you receive an error message: `MPIDI_CRAY_init: GPU_SUPPORT_ENABLED is requested, but GTL library is not linked` after running your code, delete the `build` folder and re-build the code. Please run the below command prior to building your code.  
    `export MPICH_GPU_SUPPORT_ENABLED=0`

# Build instructions, general

This distribution uses cmake and MPI.

After downloading, cd into the mpi-2dmesh-harness directory, then:  

    mkdir build  
    cd build  
    cmake  
    make

# Running the code on Perlmutter

For the examples that follow, we assume your current working directory is:
    `.../mpi_2dmesh_harness_instructional/build`

The reason is that the code will attempt to open and load a data file from disk that
is located in the ../data directory.


## Debug/interactive runs on one node

Your first parallel run with MPI should be on a single CPU node of Perlmutter. 
Since each Perlmutter CPU node has dual AMD Milan3 processors with 64 cores each, you could
in principle run up to 128-way parallel on a single CPU node. 

For CP6 in CSC 746, you will need to do parallel runs on multiple CPU nodes. For now, 
doing test runs on a single CPU node of the code at varying concurrency will be useful
in making sure the code is running correctly, etc.

Once you build the code, then hop on a single CPU node:

    salloc --nodes 1 --qos interactive --time 00:30:00 --constraint cpu --account=m3930

Then, from the build subdirectory, you may run N-way parallel as follows:

    srun -n N ./mpi_2dmesh

## Debug/interactive runs on P nodes, P <= 4

You may do interactive runs using multiple CPU nodes. This activity should be kept to a minimum
in order to keep account billing to a minimum. For doing "production runs", please use the batch queue.

NERSC imposes a limit of a maximum of 4 nodes in the interactive queue. To hop on a group of 4 nodes:
    salloc --nodes 4 --qos interactive --time 00:30:00 --constraint cpu --account=m3930

When you are granted access, you will have an interactive shell on one of the 3 nodes. 
Then, when you run your job, srun will map MPI ranks to the different nodes in round-robin fashion.

To run N-way parallel on P nodes:

    srun -n N ./mpi_2dmesh

## Running the code via the sbatch queue

Located inside the scripts subdirectory is a `run_script.sh` file that you may use to submit
a batch job. The script will iterate over a number of levels of concurrency and over the
3 potential domain decomposition strategies.

To submit a batch job, make sure your current working directory is the `mpi_2dmesh_harness_instructional/build`
subdirectory, then do the job submission as follows:

     sbatch ../scripts/run_script.sh ./mpi_2dmesh

The SLURM batch system will create a logfile in your build directory named something like 'slurm-XXXXX.out" 
where XXXXX is the job number. stderr and stdout from your code and any other system messages from the
job run will appear in that file.

To monitor the status of your job, you can either use the 'sqs' command, which shows the current state
of your batch jobs, or you can 'tail -f' the slurm-XXXXX.out file and watch its progress.

## What happens when the project runs out of hours

In the event that all hours are exhausted, there is one final workaround while we await
additional resources for the project.

You may submit your batch job to the "overrun" queue. 
See [this page](https://docs.nersc.gov/jobs/policy/#qos-cost-factor-charge-multipliers-and-discounts) for more information about the overrun queue.

To access the overrun queue, use the scripts/runs_script_overrun.sh script to submit your job. From
the build, directory, execute this command:  

    sbatch ../scripts/run_script_overrun.sh ./mpi_2dmesh

The job will be submitted to the overrun queue where it will have very low priority.

## Running the code on the VM

On the VM, use the 'mpirun' command to launch your job. E.g., to run 8-way parallel:

    mpirun -n 8 ./mpi_2dmesh

When running at P>1 concurrency, note that since there is effectively only a single core accessible to the VM,
your code will appear to be running in parallel, but the multiple concurrent ranks are being executed
in serial fashion.

# Adding your code: 3 locations

Your coding assignment consists of adding code to implement MPI-based communication
that needs to take place during the scatter and gather phases of processing, and to
also add code that computes the Sobel filter operation on data in a distributed memory
fashion.

### 1. Adding your code to sendStridedBuffer()

This method, which is called from both scatterAllTiles() and gatherAllTiles(),
is responsible for sending data from one rank to another. 

void  
sendStridedBuffer(float \*srcBuf,   
    int srcWidth, int srcHeight,   
    int srcOffsetColumn, int srcOffsetRow,  
    int sendWidth, int sendHeight,  
    int fromRank, int toRank )   

// ADD YOUR CODE HERE  

Your code will perform the sending of data using MPI\_Send(), going _fromRank_ and 
to _toRank_. The data to be sent is in _srcBuf_, which has width _srcWidth_, _srcHeight_.
Your code needs to send a subregion of _srcBuf_, where the subregion is of size
_sendWidth_ by _sendHeight_ values, and the subregion is offset from the origin of
_srcBuf_ by the values specified by _srcOffsetColumn_, _srcOffsetRow_.



### 2. Adding your code to recvStridedBuffer()

This method, which is called from both scatterAllTiles() and gatherAllTiles(),
is responsible for receiving data moving from one rank to another.

void  
recvStridedBuffer(float \*dstBuf,  
    int dstWidth, int dstHeight,  
    int dstOffsetColumn, int dstOffsetRow,  
    int expectedWidth, int expectedHeight,  
    int fromRank, int toRank )   

// ADD YOUR CODE HERE

Your code will perform the receiving of data using MPI\_Recv(), where inbound data
is coming _fromRank_ and is destined for _toRank_. The data that arrives will be of size 
_expectedWidth_ by _expectedHeight_ values.  This incoming data is to be placed into 
the subregion of _dstBuf_ that has an origin at _dstOffsetColumn, dstOffsetRow_, and 
that is _expectedWidth, expectedHeight_ in size.


### 3. Adding your sobel filtering code

The intention here is for you to transplant part of your code from HW5 into this
MPI-based code. There are two locations where you will need to add code.

First, inside the _sobelAllTiles_ routine is a doubly-nested loop that iterates over tiles.
Inside the inner loop is a conditional that checks if _t->tileRank == myrank_, and if
so, you need to add a call to your _do_sobel_filtering()_ method from your sobel_cpu.cpp
code. You will invoke that method to process a tile's worth of data, passing in the
tile's _inputBuffer_ as the input data, and the tile's _outputBuffer_ to receive the
sobel filtered results.

Note: even though your _do_sobel_filtering()_ code was parallelized using OpenMP, we are
*not* using OpenMP parallelism in this project.

Second, you need to transplant your two methods, _do_sobel_filtering()_ and _sobel_filter_pixel()_ from your sobel\_cpu.cpp code into the mpi\_2dmesh.cpp code, where they will be invoked
to do the Sobel computation.

# Information about data files

Zebra file dimensions 
* Original: 3556 2573
* 4x Augmented: 7112 5146

* zebra-gray-int8.dat - raw 8-bit grayscale pixel values from the Zebra_July_2008-1.jpg image
* zebra-gray-int8-4x.dat - raw 8-bit grayscale pixel values from the Zebra_July_2008-1.jpg image but 
augmented 2x in each direction

Source file:  Zebra_July_2008-1.jpg, obtained from Wikimedia commons, https://commons.wikimedia.org/wiki/File:Zebra_July_2008-1.jpg

# python display script

imshow.py - a python script to display the raw 8-bit pixel values in grayscale. 

Usage:  
    python imshow.py filename-of-raw-8bit-bytes int-cols-width int-rows-height

To use this script from Perlmutter, first please enable X11-tunneling through ssh before you
log in. E.g., "ssh -Y username@perlmutter-p1.nersc.gov"

On Perlmutter, before using this script, please load the Python module:  
> module load python

Some people on Mac systems have reported problems with the remote image display
not working from Perlmutter. This is likely the result of either (1) forgetting to enable
X tunneling through ssh by forgetting to add the -Y option to ssh, or (2) a
Mac-side Quartz configuration issue. 

If this issue affects you, you might want to consider trying NX to remote desktop
to Perlmutter. See https://docs.nersc.gov/connect/nx/ for more information.
