# csc746-tp-ray-tracing-omp
Accelerating Ray Tracing Using Parallelization

This project is an enhanced version of the "Ray Tracing in One Weekend" series, with a focus on high-performance computing. Our goal is to achieve significant speedup in ray tracing by parallelizing the computation on both CPU and GPU nodes.

## Features

- **Ray Tracing Basics**: Implements the foundational concepts of ray tracing as described in the "Ray Tracing in One Weekend" book.
- **High Performance Computing**: Optimized for speed and efficiency, leveraging modern hardware capabilities.
- **Parallelization**:
  - **CPU Node Parallelization**: Utilizes multi-threading to distribute the workload across multiple CPU cores.
  - **GPU Node Parallelization**: Offloads computation to the GPU, taking advantage of its massive parallel processing power.

## Getting Started

### Prerequisites

- A C++ compiler with C++11 support
- OpenMP for CPU parallelization

### Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/csc746-tp-ray-tracing-omp
   cd csc746-tp-ray-tracing-omp
   ```

2. Build the project:
   ```bash
   cmake -B build/
   cmake --build build/
   ```

### Running the Application
  ```bash
  OMP_NUM_THREADS=1 ./build/ray-tracing-omp > image.ppm
  OMP_NUM_THREADS=2 ./build/ray-tracing-omp > image.ppm
  OMP_NUM_THREADS=4 ./build/ray-tracing-omp > image.ppm
  OMP_NUM_THREADS=8 ./build/ray-tracing-omp > image.ppm
  ```

## Performance

By parallelizing the ray tracing algorithm, we aim to achieve significant speedups compared to the original single-threaded implementation. Benchmarks and performance metrics will be provided in future updates.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Inspired by the [_Ray Tracing in One Weekend_](https://raytracing.github.io/books/RayTracingInOneWeekend.html) series by Peter Shirley.
