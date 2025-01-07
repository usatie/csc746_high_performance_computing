# sobel-harness-instructional

This directory contains the code harnesses for doing three different implementation of a Sobel edge detection
filter. The three implementations are:

* CPU only in C++, with your added OpenMP parallelism
* GPU in CUDA
* GPU in C++ with OpenMP Device Offload for running a kernel on the GPU

# Build instructions - general

This assignment requires use of the Nvidia compilers as follows:

    module load PrgEnv-nvidia
    export CC=cc
    export CXX=CC 

Then, once your environment is set up, then:

    mkdir build  
    cd build  
    cmake ../  -Wno-dev
    make

It is OK to do builds on the login node once you have set up the environment above.  
All code should be executed on a GPU node.

# Running the experiments

The experiments are run using the Slurm batch system. The batch scripts are provided in the directory.
    sbatch ./job-cpu
    sbatch ./job-gpu
    sbatch ./job-gpu-offload

# python display script

imshow.py - a python script to display the raw 8-bit pixel values in grayscale. 

Usage:  

    python imshow.py filename-of-raw-8bit-bytes int-cols-width int-rows-height


# eof
