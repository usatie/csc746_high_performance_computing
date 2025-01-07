# High Performance & Distributed Computing Projects

This repository showcases my work from CSC746 (High Performance Computing), demonstrating my experience with distributed systems, parallel computing, and performance optimization. Each project highlights different aspects of high-performance computing.

## Coding Projects

### CP1-2: Fundamental Performance Studies
- **Array Access Optimization**:
  - Implemented three array access methods:
    1. Direct array access (A[i])
    2. Indirect array access (A[B[i]])
    3. Vectorized array access (SIMD)
  - Analyzed performance impact of memory access patterns
  - Demonstrated how indirect access creates irregular memory patterns
- **Performance Analysis**: 
  - Achieved 2.8x speedup with vectorized vs indirect access
  - Identified cache miss rates as key performance factor
  - Studied impact of problem sizes from 1K to 64M elements
- **Compiler Optimization Study**:
  - Compared performance across optimization levels (-O0 to -O3)
  - Analyzed compiler's ability to auto-vectorize code
  - Documented impact of -ffast-math and SIMD flags
- **Matrix Multiplication Optimization**:
  - Implemented basic and blocked matrix multiplication
  - Analyzed cache performance with different block sizes
  - Validated results against CBLAS reference implementation

### CP3: Vector-Matrix Operations
- **Algorithm Optimization**: 
  - Developed OpenMP-parallelized vector-matrix multiplication
  - Achieved near-linear scaling up to 16 threads
  - Validated against CBLAS reference implementation
- **Performance Analysis**: Used LIKWID to analyze cache behavior and memory bandwidth utilization

### CP4: Parallel Matrix Operations with OpenMP
- **Concurrent Programming**: 
  - Implemented blocked matrix multiplication with OpenMP parallelization
  - Achieved 12.8x speedup on 16 cores through cache-aware blocking
- **Performance Analysis**: 
  - Used LIKWID performance counters to measure memory bandwidth and FLOPS
  - Identified and resolved cache coherency bottlenecks

### CP5: GPU-Accelerated Image Processing
- **Heterogeneous Computing**:
  - Implemented Sobel edge detection using:
    1. CPU with OpenMP (baseline)
    2. GPU with CUDA 
    3. GPU with OpenMP device offload
- **Performance Results**:
  - Achieved 20x speedup with CUDA implementation
  - Analyzed memory transfer overhead vs. computation time
  - Optimized thread block sizes for maximum GPU utilization

### CP6: Distributed Image Processing with MPI
- **Distributed Systems Design**: 
  - Implemented scatter-gather pattern for distributed Sobel filter
  - Developed three domain decomposition strategies:
    1. Row-slab decomposition
    2. Column-slab decomposition  
    3. 2D tiled decomposition
- **Scalability**: Linear scaling demonstrated up to 64 processes across multiple nodes
- **Communication Optimization**: Minimized overhead through efficient boundary exchange

## Technical Skills Demonstrated

### Programming Languages
- Advanced C++ (primary language for all projects)
- CUDA for GPU programming
- OpenMP for shared memory parallelism
- MPI for distributed computing

### High Performance Computing
- Distributed Systems Programming
- Parallel Algorithm Design
- Performance Analysis and Optimization
- Cache/Memory Hierarchy Optimization

### Tools & Technologies
- LIKWID Performance Monitoring
- CUDA Toolkit
- OpenMP 
- MPI
- CMake Build System

## Performance Results
Each project includes detailed performance analysis and optimization results. For example:
- CP6 demonstrates linear scaling up to 64 processes across multiple nodes
- CP5 shows 20x speedup using GPU acceleration
- CP4 achieves 12.8x speedup with 16 threads and optimized blocking
- CP3 shows near-linear scaling with OpenMP parallelization
- CP1-2 demonstrate 3x speedup through memory access optimization