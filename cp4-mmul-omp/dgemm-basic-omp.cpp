#include <iostream>
#include <omp.h>
#include "likwid-stuff.h"

const char* dgemm_desc = "Basic implementation, OpenMP-enabled, three-loop dgemm.";

/*
 * This routine performs a dgemm operation
 *  C := C + A * B
 * where A, B, and C are n-by-n matrices stored in row-major format.
 * On exit, A and B maintain their input values.
 */
void square_dgemm(int n, double* A, double* B, double* C) 
{
   // insert your code here: implementation of basic matrix multiply with OpenMP parallelism enabled
   // be sure to include LIKWID_MARKER_START(MY_MARKER_REGION_NAME) inside the block of parallel code,
   // but before your matrix multiply code, and then include LIKWID_MARKER_STOP(MY_MARKER_REGION_NAME)
   // after the matrix multiply code but before the end of the parallel code block.
#pragma omp parallel for
   for (int i = 0; i < n; ++i) {
#ifdef LIKWID_PERFMON
      LIKWID_MARKER_START(MY_MARKER_REGION_NAME);
#endif
      for (int j = 0; j < n; ++j) {
         double cij = C[i*n+j];   /* cij = C[i][j] */
         for( int k = 0; k < n; k++ )
            cij += A[i*n+k] * B[k*n+j]; /* cij += A[i][k]*B[k][j] */
         C[i*n+j] = cij; /* C[i][j] = cij */
      }
#ifdef LIKWID_PERFMON
      LIKWID_MARKER_STOP(MY_MARKER_REGION_NAME);
#endif
   }
}
