#include <omp.h>
#include <stdio.h>
#include <time.h>

static long num_steps = 100000;
double step;

int main()
{
#define MAX_THREADS 8
	double pi = 0.0, sum[MAX_THREADS] = {0.0};
	// start precise timer
	struct timespec start, end;
	step = 1.0 / (double)num_steps;
	clock_gettime(CLOCK_REALTIME, &start);
#pragma omp parallel
	{
		int id = omp_get_thread_num();
		int num_threads = omp_get_num_threads();
		double x;
		for (int i = id; i < num_steps; i += num_threads)
		{
			x = (i + 0.5) * step;
			sum[id] += 4.0 / (1.0 + x * x) * step;
		}
	}
	for (int i = 0; i < MAX_THREADS; i++) {
		pi += sum[i];
	}
	// stop timer
	clock_gettime(CLOCK_REALTIME, &end);
	long elapsed_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
	printf("Elapsed time: %ldus\n", elapsed_us);
	return pi - 3.14159265358979323846;
}
