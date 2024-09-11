// #include <iostream>
const char *dgemm_desc = "Blocked dgemm.";

/* This routine reads a block of size b-by-b from a matrix M
 * and stores it in a buffer block.
 * The block is located starting at (row * b, col * b) in M,
 * where M is an n-by-n matrix stored in column-major format.
 */
static void blockread(double *block, double *M, int row, int col, int n,
                      int b) {
  for (int j = 0; j < b; j++) {
    for (int i = 0; i < b; i++) {
      // block : b*b
      // M   : n*n
      // block[i, j] = M_row_col[i, j]
      block[i + j * b] = M[(row * b + i) + (col * b + j) * n];
    }
  }
}

/* This routine writes a block of size b-by-b from a buffer block
 * back into a matrix M.
 * The block is placed starting at (row * b, col * b) in M,
 * where M is an n-by-n matrix stored in column-major format.
 */
static void blockwrite(double *block, double *M, int row, int col, int n,
                       int b) {
  for (int j = 0; j < b; j++) {
    for (int i = 0; i < b; i++) {
      // block : b*b
      // M   : n*n
      // M_row_col[i, j] = block[i, j]
      M[(row * b + i) + (col * b + j) * n] = block[i + j * b];
    }
  }
}

/*
 * This routine performs a dgemm operation
 *  C := C + A * B
 * where A, B, and C are n-by-n matrices stored in column-major format.
 * On exit, A and B maintain their input values.
 */
static void square_dgemm(int n, double *A, double *B, double *C) {
  // insert your code here: implementation of basic matrix multiple
  for (int j = 0; j < n; j++) {
    for (int k = 0; k < n; k++) {
      for (int i = 0; i < n; i++) {
        C[i + j * n] += A[i + k * n] * B[k + j * n];
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
 * where A, B, and C are n-by-n matrices stored in column-major format.
 * On exit, A and B maintain their input values. */
void square_dgemm_blocked(int n, int block_size, double *A, double *B,
                          double *C) {
  // insert your code here
#define MAX_BLOCK (64 * 64)
  static double aBlock[MAX_BLOCK], bBlock[MAX_BLOCK], cBlock[MAX_BLOCK];
  for (int i = 0; i < n / block_size; i++) {
    for (int j = 0; j < n / block_size; j++) {
      blockread(cBlock, C, i, j, n, block_size);
      for (int k = 0; k < n / block_size; k++) {
        blockread(aBlock, A, i, k, n, block_size);
        blockread(bBlock, B, k, j, n, block_size);
        // C[i, j] += A[i, k] * B[k, j];
        square_dgemm(block_size, aBlock, bBlock, cBlock);
      }
      blockwrite(cBlock, C, i, j, n, block_size);
    }
  }
}
