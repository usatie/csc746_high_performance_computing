cmake_minimum_required(VERSION 3.16)
project(ray-tracing-omp LANGUAGES C CXX)

set(CMAKE_BUILD_TYPE "Release")

find_package(SDL2 QUIET)
find_package(OpenMP REQUIRED)

set(SOURCE_FILE src/main.cpp)
set(OMP_COLLAPSE "0" "1")

if (SDL2_FOUND)
	add_executable(ray-tracing-omp
		src/main.cpp
	)
	target_link_options(ray-tracing-omp PRIVATE -fopenmp)
	target_compile_definitions(ray-tracing-omp PRIVATE -DSDL2)
	target_link_libraries(ray-tracing-omp PUBLIC OpenMP::OpenMP_CXX)
	target_compile_definitions(ray-tracing-omp PUBLIC OMP_COLLAPSE=0) # We don't use collapse for SDL2
	target_include_directories(ray-tracing-omp PRIVATE ${SDL2_INCLUDE_DIRS})
	target_link_libraries(ray-tracing-omp PRIVATE ${SDL2_LIBRARIES})
else ()
	# LIKWID is required for actual experiments!
	find_package(LIKWID REQUIRED)

	# We require the GNU compiler for this
	if (NOT "${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
	    set(Prg-Intel "PrgEnv-intel")
	    set(Prg-Clang "PrgEnv-cray")
	    message(WARNING
		    "Must use GNU Compiler for submission. Make sure you ran:\n"
		    "module swap ${Prg-${CMAKE_C_COMPILER_ID}} PrgEnv-gnu")
	endif ()

	# Create target for each combination
	foreach(COLLAPSE ${OMP_COLLAPSE})
	    message(STATUS "Configuring with COLLAPSE=${COLLAPSE}")
	    # Generate unique name
	    if (COLLAPSE)
	      set(TARGET_NAME "ray-tracing-omp_collapse")
	    else ()
	      set(TARGET_NAME "ray-tracing-omp")
	    endif ()

	    # Add executable file target
	    add_executable(${TARGET_NAME} ${SOURCE_FILE})
	    target_link_libraries(${TARGET_NAME} PUBLIC OpenMP::OpenMP_CXX)

	    # Configure OpenMP parameters
	    target_compile_definitions(${TARGET_NAME} PUBLIC
		OMP_COLLAPSE=${COLLAPSE})

	    # Compile options
	    target_compile_options(${TARGET_NAME} PRIVATE -fopenmp -march=native)

	    target_include_directories(${TARGET_NAME} PRIVATE ${LIKWID_INCLUDE_DIR})
	    target_compile_definitions(${TARGET_NAME} PRIVATE -DLIKWID_PERFMON)
	    target_include_directories(${TARGET_NAME} PRIVATE ${LIKWID_INCLUDE_DIR})
	    target_compile_definitions(${TARGET_NAME} PRIVATE -DLIKWID_PERFMON)
	    target_link_libraries(${TARGET_NAME} PRIVATE ${LIKWID_LIBRARIES})

	endforeach()
endif()
