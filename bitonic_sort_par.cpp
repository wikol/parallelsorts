#include <omp.h>
#include "bitonic_sort_par.h"

namespace bitonic_sort_par_internal {

void bitonic_sort_seq(int *T, unsigned n, bool ascending);
void bitonic_sort(int *T, unsigned n, bool ascending, unsigned num_threads);

void bitonic_merge_seq(int *T, unsigned n, bool ascending) {
  if (n == 1)
    return;

  unsigned m = 1;
  while((m << 1) < n) {
    m <<= 1;
  }

  if (ascending) {
    for (unsigned i = 0; i < n - m; i++) {
      if (T[i] > T[i + m]) {
        int tmp = T[i];
        T[i] = T[i + m];
        T[i + m] = tmp;
      }
    }
  } else {
    for (unsigned i = 0; i < n - m; i++) {
      if (T[i] < T[i + m]) {
        int tmp = T[i];
        T[i] = T[i + m];
        T[i + m] = tmp;
      }
    }
  }
  // Recursive calls
  bitonic_merge_seq(T, m, ascending);
  bitonic_merge_seq(T + m, n - m, ascending);
}

void bitonic_merge(int *T, unsigned n, bool ascending, unsigned num_threads) {
  if (num_threads == 1) {
    bitonic_merge_seq(T, n, ascending);
    return;
  }

  if (n == 1)
    return;

  unsigned m = 1;
  while((m << 1) < n) {
    m <<= 1;
  }

  if (ascending) {
    #pragma omp parallel for num_threads(num_threads)
    for (unsigned i = 0; i < n - m; i++) {
      if (T[i] > T[i + m]) {
        int tmp = T[i];
        T[i] = T[i + m];
        T[i + m] = tmp;
      }
    }
  } else {
    #pragma omp parallel for num_threads(num_threads)
    for (unsigned i = 0; i < n - m; i++) {
      if (T[i] < T[i + m]) {
        int tmp = T[i];
        T[i] = T[i + m];
        T[i + m] = tmp;
      }
    }
  }
  // Recursive calls
  unsigned num_threads_first_half = (num_threads >> 1);
  unsigned num_threads_second_half = num_threads - num_threads_first_half;
  #pragma omp parallel sections
  {
    #pragma omp section
    { bitonic_merge(T, m, ascending, num_threads_first_half); }
    #pragma omp section
    { bitonic_merge(T + m, n - m, ascending, num_threads_second_half); }
  }
}

void bitonic_sort_seq(int *T, unsigned n, bool ascending) {
  if (n == 1)
    return;

  unsigned half_n = n >> 1;
  // Recursive calls
  bitonic_sort_seq(T, half_n, !ascending);
  bitonic_sort_seq(T + half_n, n - half_n, ascending);

  // Bitonic merge
  bitonic_merge_seq(T, n, ascending);
}

void bitonic_sort(int *T, unsigned n, bool ascending, unsigned num_threads) {
  if (num_threads == 1) {
    bitonic_sort_seq(T, n, ascending);
    return;
  }

  if (n == 1)
    return;

  unsigned half_n = n >> 1;
  // Recursive calls
  unsigned num_threads_first_half = (num_threads >> 1);
  unsigned num_threads_second_half = num_threads - num_threads_first_half;
  #pragma omp parallel sections
  {
    #pragma omp section
    { bitonic_sort(T, half_n, !ascending, num_threads_first_half); }
    #pragma omp section
    { bitonic_sort(T + half_n, n - half_n, ascending, num_threads_second_half); }
  }

  // Bitonic merge
  bitonic_merge(T, n, ascending, num_threads);
}
}

/* Parallel bitonic sort using OpenMP */
void bitonic_sort_par(int *T, unsigned n) {
  if(n > 0) {
    unsigned num_threads = omp_get_max_threads();
    bitonic_sort_par_internal::bitonic_sort(T, n, true, num_threads);
  }
}
