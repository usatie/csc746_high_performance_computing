run: build
	cd build && ./sobel_cpu

.PHONY: build
build: sobel_cpu.cpp
	cmake -B build -Wno-dev
	cmake --build build

.PHONY: setup
setup:
	module load PrgEnv-nvidia
	export CC=cc
	export CXX=CC
