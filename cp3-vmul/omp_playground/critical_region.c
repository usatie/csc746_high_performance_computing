#include <omp.h>
#include <stdio.h>
#include <time.h>

static long num_steps = 10000000;
double step;

int main()
{
	double x, pi, sum = 0.0;
	// start precise timer
	struct timespec start, end;
	step = 1.0 / (double)num_steps;
	clock_gettime(CLOCK_REALTIME, &start);
#pragma omp parallel
	{
		double x, psum = 0.0;
		int id = omp_get_thread_num();
		int num_threads = omp_get_num_threads();
		for (int i = id; i < num_steps; i += num_threads)
		{
			x = (i + 0.5) * step;
			psum += 4.0 / (1.0 + x * x) * step;
		}
#pragma omp critical
		sum += psum;
	}
	pi = sum;
	// stop timer
	clock_gettime(CLOCK_REALTIME, &end);
	long elapsed_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
	printf("Elapsed time: %ldus\n", elapsed_us);
	return pi - 3.14159265358979323846;
}
