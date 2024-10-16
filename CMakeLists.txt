cmake_minimum_required(VERSION 3.16)
project(ray-tracing-omp LANGUAGES C CXX)

add_executable(ray-tracing-omp
	src/main.cpp
	src/vec3.h
)
