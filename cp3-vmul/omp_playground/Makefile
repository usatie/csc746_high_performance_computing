all: serial_sum critical_region loop_reduction

serial_sum: serial_sum.c
	gcc -fopenmp -o serial_sum serial_sum.c -O0

critical_region: critical_region.c
	gcc -fopenmp -o critical_region critical_region.c -O0

critical_region_sequential: critical_region_sequential.c
	gcc -fopenmp -o critical_region_sequential critical_region_sequential.c -O0

loop_reduction: loop_reduction.c
	gcc -fopenmp -o loop_reduction loop_reduction.c -O0

bench_serial_sum: serial_sum
	OMP_NUM_THREADS=1 ./serial_sum
	OMP_NUM_THREADS=2 ./serial_sum
	OMP_NUM_THREADS=4 ./serial_sum
	OMP_NUM_THREADS=8 ./serial_sum

bench_critical_region: critical_region
	OMP_NUM_THREADS=1 ./critical_region
	OMP_NUM_THREADS=2 ./critical_region
	OMP_NUM_THREADS=4 ./critical_region
	OMP_NUM_THREADS=8 ./critical_region

bench_critical_region_sequential: critical_region_sequential
	OMP_NUM_THREADS=1 ./critical_region_sequential
	OMP_NUM_THREADS=2 ./critical_region_sequential
	OMP_NUM_THREADS=4 ./critical_region_sequential
	OMP_NUM_THREADS=8 ./critical_region_sequential

bench_loop_reduction: loop_reduction
	OMP_NUM_THREADS=1 ./loop_reduction
	OMP_NUM_THREADS=2 ./loop_reduction
	OMP_NUM_THREADS=4 ./loop_reduction
	OMP_NUM_THREADS=8 ./loop_reduction

bench: bench_serial_sum bench_critical_region bench_loop_reduction bench_critical_region_sequential

clean:
	rm -f serial_sum critical_region loop_reduction

re: clean all
