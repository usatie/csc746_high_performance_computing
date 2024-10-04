const char *dgemv_desc = "Basic implementation of matrix-vector multiply.";

/*
 * This routine performs a dgemv operation
 * Y :=  A * X + Y
 * where A is n-by-n matrix stored in row-major format, and X and Y are n by 1
 * vectors. On exit, A and X maintain their input values.
 */
void my_dgemv(int n, double *A, double *x, double *y) {
  // insert your code here: implementation of basic matrix multiply
#ifdef LOOP_IJ
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
#elif LOOP_JI
  for (int j = 0; j < n; j++) {
    for (int i = 0; i < n; i++) {
#endif
      y[i] += A[i * n + j] * x[j];
    }
  }
}
