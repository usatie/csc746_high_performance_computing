cmake_minimum_required(VERSION 3.16)
project(ray-tracing-omp LANGUAGES C CXX)

find_package(SDL2)

add_executable(ray-tracing-omp
	src/main.cpp
)
target_link_options(ray-tracing-omp PRIVATE -fopenmp)
if (SDL2_FOUND)
	target_compile_definitions(ray-tracing-omp PRIVATE -DSDL2)
	target_include_directories(ray-tracing-omp PRIVATE ${SDL2_INCLUDE_DIRS})
	target_link_libraries(ray-tracing-omp PRIVATE ${SDL2_LIBRARIES})
endif()
target_compile_options(ray-tracing-omp PRIVATE -fopenmp -march=native)
