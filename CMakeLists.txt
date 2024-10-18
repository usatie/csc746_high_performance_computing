cmake_minimum_required(VERSION 3.16)
project(ray-tracing-omp LANGUAGES C CXX)

add_executable(ray-tracing-omp
	src/main.cpp
)
target_link_options(ray-tracing-omp PRIVATE -fopenmp)
target_compile_options(ray-tracing-omp PRIVATE -fopenmp -march=native)
