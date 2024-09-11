//
// (C) 2021, E. Wes Bethel
// benchmark-* hardness for running different versions of matrix multiply
//    over different problem sizes
//
// usage: no command line arguments
// set problem sizes, block sizes in the code below

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

#include <cmath> // For: fabs

#include <cblas.h>
#include <string.h>

// external definitions for mmul's
extern void square_dgemm(int, double *, double *, double *);
extern void square_dgemm_blocked(int, int, double *, double *, double *);
extern const char *dgemm_desc;

void reference_dgemm(int n, double alpha, double *A, double *B, double *C) {
  cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, n, n, n, alpha, A, n,
              B, n, 1., C, n);
}

void fill(double *p, int n) {
  static std::random_device rd;
  static std::default_random_engine gen(rd());
  static std::uniform_real_distribution<> dis(-1.0, 1.0);
  for (int i = 0; i < n; ++i)
    p[i] = 2 * dis(gen) - 1;
}

bool check_accuracy(double *A, double *Anot, int nvalues) {
  double eps = 1e-5;
  for (size_t i = 0; i < nvalues; i++) {
    if (fabsf(A[i] - Anot[i]) > eps) {
      return false;
    }
  }
  return true;
}

/* The benchmarking program */
int main(int argc, char **argv) {
  std::cout << "Description:\t" << dgemm_desc << std::endl << std::endl;

  std::cout << std::fixed << std::setprecision(6);

  std::vector<int> test_sizes{64, 128, 256, 512, 1024, 2048};
  std::vector<int> block_sizes{2, 16, 32, 64};

  int n_problems = test_sizes.size();

  /* For each test size */
  for (int n : test_sizes) {
    printf("Working on problem size N=%d \n", n);

#ifdef BLOCKED
    printf("Blocked DGEMM \n");
    for (int b : block_sizes) {
#endif

      // allocate memory for 6 NxN matrics
      std::vector<double> buf(6 * n * n);
      double *A = buf.data() + 0;
      double *B = A + n * n;
      double *C = B + n * n;
      double *Acopy = C + n * n;
      double *Bcopy = Acopy + n * n;
      double *Ccopy = Bcopy + n * n;

      // load up matrics with some random numbers
      fill(A, n * n);
      fill(B, n * n);
      fill(C, n * n);

      // make copies of A, B, C for use in verification of results
      memcpy((void *)Acopy, (const void *)A, sizeof(double) * n * n);
      memcpy((void *)Bcopy, (const void *)B, sizeof(double) * n * n);
      memcpy((void *)Ccopy, (const void *)C, sizeof(double) * n * n);

      // insert timer code here
      std::chrono::time_point<std::chrono::high_resolution_clock> start_time =
          std::chrono::high_resolution_clock::now();

#ifdef BLOCKED
      square_dgemm_blocked(n, b, A, B, C);
#else
    square_dgemm(n, A, B, C);
#endif

      // insert timer code here
      std::chrono::time_point<std::chrono::high_resolution_clock> end_time =
          std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> elapsed_time = end_time - start_time;
#ifdef BLOCKED
      printf("(block_size: %2d) ", b);
#endif
      std::cout << "Elapsed time: " << elapsed_time.count() << " " << std::endl;

      reference_dgemm(n, 1.0, Acopy, Bcopy, Ccopy);

      // compare your C with that computed by BLAS
      if (check_accuracy(Ccopy, C, n * n) == false)
        printf(
            " Error: your answer is not the same as that computed by BLAS. \n");

#ifdef BLOCKED
    } // end loop over block sizes
#endif

  } // end loop over problem sizes

  return 0;
}

// EOF
