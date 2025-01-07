#include <omp.h>
#include <stdio.h>
#include <time.h>

static long num_steps = 100000;
double step;

int main()
{
	int i;
	double x, pi, sum = 0.0;
	// start precise timer
	struct timespec start, end;
	step = 1.0 / (double)num_steps;
	clock_gettime(CLOCK_REALTIME, &start);
#pragma omp parallel
	{
		double x;
#pragma omp for reduction(+:sum)
		for (int i = 0; i < num_steps; i++)
		{
			x = (i + 0.5) * step;
			sum = sum + 4.0 / (1.0 + x * x) * step;
		}
	}
	pi = sum;
	// stop timer
	clock_gettime(CLOCK_REALTIME, &end);
	long elapsed_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
	printf("Elapsed time: %ldus\n", elapsed_us);
	return pi - 3.14159265358979323846;
}
