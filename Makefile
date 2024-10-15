local:
	mkdir -p build
	cd build && cmake .. && make

run: local run-basic run-blas run-blocked

run-basic:
	./build/benchmark-basic-omp

run-blas:
	./build/benchmark-blas

run-blocked:
	./build/benchmark-blocked-omp

remote:
	bash remote-build.sh

remote-test:
	bash remote-test.sh
