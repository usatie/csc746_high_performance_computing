#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>
#include <string.h>
#include <algorithm>



void 
setup(int64_t N, uint64_t A[])
{
   printf(" inside sum_indirect problem_setup, N=%lld \n", N);
   std::vector<int> walk_order(N);
   for (int64_t i = 0; i < N; i++)
   {
      walk_order[i] = i;
   }
   std::shuffle(walk_order.begin(), walk_order.end(), std::default_random_engine());
   // random walk
   int64_t index = walk_order[0];
   for (int64_t i = 0; i < N; i++)
   {
      int64_t next_index = walk_order[(i+1) % N];
      A[index] = next_index;
      index = next_index;
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

