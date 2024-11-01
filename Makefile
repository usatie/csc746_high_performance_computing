run:
	export OMP_PLACES=threads
	export OMP_PROC_BIND=spread
	cd build && OMP_NUM_THREADS=1 ./sobel_cpu
	cd build && OMP_NUM_THREADS=2 ./sobel_cpu
	cd build && OMP_NUM_THREADS=4 ./sobel_cpu
	cd build && OMP_NUM_THREADS=8 ./sobel_cpu
	cd build && OMP_NUM_THREADS=16 ./sobel_cpu

build: sobel_cpu.cpp
	cmake -B build -Wno-dev
	cmake --build build

.PHONY: setup
setup:
	module load PrgEnv-nvidia
	export CC=cc
	export CXX=CC
