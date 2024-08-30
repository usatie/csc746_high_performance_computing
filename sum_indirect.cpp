#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>
#include <string.h>



void 
setup(int64_t N, uint64_t A[])
{
   printf(" inside sum_indirect problem_setup, N=%lld \n", N);
   for (int64_t i = 0; i < N; i++)
   {
      A[i] = lrand48() % N;
   }
}

int64_t
sum(int64_t N, uint64_t A[])
{
   printf(" inside sum_indirect perform_sum, N=%lld \n", N);
   int64_t sum = 0;
   for (int64_t i = 0; i < N; i++)
   {
      int64_t indirect_index = A[i];
      sum += A[indirect_index];
   }

   return sum;
}

