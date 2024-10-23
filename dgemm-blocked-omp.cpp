#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include <vector>
#include "likwid-stuff.h"

const char* dgemm_desc = "Blocked dgemm, OpenMP-enabled";


/* This routine reads a block of size b-by-b from a matrix M
 * and stores it in a buffer block.
 * The block is located starting at (row * b, col * b) in M,
 * where M is an n-by-n matrix stored in row-major format.
 */
static void blockread(double *block, double *M, int row, int col, int n,
                      int b) {
  for (int i = 0; i < b; i++) {
    for (int j = 0; j < b; j++) {
      // block : b*b
      // M   : n*n
      // block[i, j] = M_row_col[i, j]
      block[i * b + j] = M[(row * b + i) * n + (col * b + j)];
    }
  }
}

/* This routine writes a block of size b-by-b from a buffer block
 * back into a matrix M.
 * The block is placed starting at (row * b, col * b) in M,
 * where M is an n-by-n matrix stored in row-major format.
 */
static void blockwrite(double *block, double *M, int row, int col, int n,
                       int b) {
  for (int i = 0; i < b; i++) {
    for (int j = 0; j < b; j++) {
      // block : b*b
      // M   : n*n
      // M_row_col[i, j] = block[i, j]
      M[(row * b + i) * n + (col * b + j)] = block[i * b + j];
    }
  }
}

/*
 * This routine performs a dgemm operation
 *  C := C + A * B
 * where A, B, and C are n-by-n matrices stored in row-major format.
 * On exit, A and B maintain their input values.
 */
static void square_dgemm(int n, double *A, double *B, double *C) {
  // insert your code here: implementation of basic matrix multiple
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      for (int k = 0; k < n; k++) {
        C[i * n + j] += A[i * n + k] * B[k * n + j];
      }
    }
  }
}

/* This routine prints a matrix of n-by-n
 * For debug.
 * */
//  static void print_matrix(double *M, int n)
//  {
//    std::cout << "[" << std::endl;
//    for (int i = 0; i < n; i++) {
//      std::cout << "  [";
//      for (int j = 0; j < n; j++) {
//        std::cout << M[i + j*n] << ", ";
//      }
//      std::cout << "]" << std::endl;
//    }
//    std::cout << "]" << std::endl;
//  }


/* This routine performs a dgemm operation
 *  C := C + A * B
 * where A, B, and C are n-by-n matrices stored in row-major format.
 * On exit, A and B maintain their input values. */
void square_dgemm_blocked(int n, int block_size, double* A, double* B, double* C)
{
   // insert your code here: implementation of blocked matrix multiply with copy optimization and OpenMP parallelism enabled

   // be sure to include LIKWID_MARKER_START(MY_MARKER_REGION_NAME) inside the block of parallel code,
   // but before your matrix multiply code, and then include LIKWID_MARKER_STOP(MY_MARKER_REGION_NAME)
   // after the matrix multiply code but before the end of the parallel code block.

  // insert your code here
  int Nb = n / block_size;
#pragma omp parallel
{
  std::vector<double> a(block_size * block_size), b(block_size * block_size),
      c(block_size * block_size);
  double *aBlock = a.data(), *bBlock = b.data(), *cBlock = c.data();
#pragma omp for
  for (int i = 0; i < Nb; i++) {
#ifdef LIKWID_PERFMON
    LIKWID_MARKER_START(MY_MARKER_REGION_NAME);
#endif
    for (int j = 0; j < Nb; j++) {
      blockread(cBlock, C, i, j, n, block_size);
      for (int k = 0; k < Nb; k++) {
        blockread(aBlock, A, i, k, n, block_size);
        blockread(bBlock, B, k, j, n, block_size);
        // C[i, j] += A[i, k] * B[k, j];
        square_dgemm(block_size, aBlock, bBlock, cBlock);
      }
      blockwrite(cBlock, C, i, j, n, block_size);
    }
#ifdef LIKWID_PERFMON
    LIKWID_MARKER_STOP(MY_MARKER_REGION_NAME);
#endif
  }
}
}
