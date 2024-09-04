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
      A[i] = (i + 1) % N;
   }
   for (int64_t i = 0; i < N; i++)
   {
      int64_t rand_index = lrand48() % N;
      std::swap(A[i], A[rand_index]);
   }
}

int64_t
sum(int64_t N, uint64_t A[])
{
   printf(" inside sum_indirect perform_sum, N=%lld \n", N);
   int64_t sum = 0, index = 0;
   for (int64_t i = 0; i < N; i++)
   {
      int64_t tmp = A[index];
      sum += tmp;
      index = tmp;
   }

   return sum;
}

