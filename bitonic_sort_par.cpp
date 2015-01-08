#include "bitonic_sort_par.h"

/* Parallel bitonic sort using OpenMP */
void bitonic_sort_par(int *T, unsigned n) {
  bitonic_sort_par_internal::bitonic_sort(T, n, true);
}

namespace bitonic_sort_par_internal {

void bitonic_merge_seq(int *T, unsigned n, bool ascending) {
  if (n == 1)
    return;

  unsigned half_n = n >> 1;
  if (ascending) {
    for (unsigned i = 0; i < n - half_n; i++) {
      if (T[i] > T[i + half_n]) {
        int tmp = T[i];
        T[i] = T[i + half_n];
        T[i + half_n] = tmp;
      }
    }
  } else {
    for (unsigned i = 0; i < n - half_n; i++) {
      if (T[i] < T[i + half_n]) {
        int tmp = T[i];
        T[i] = T[i + half_n];
        T[i + half_n] = tmp;
      }
    }
  }
  // Recursive calls
  bitonic_sort_seq(T, half_n, ascending);
  bitonic_sort_seq(T + half_n, half_n, ascending);
}

void bitonic_merge(int *T, unsigned n, bool ascending) {
  if (n == 1)
    return;

  unsigned half_n = n >> 1;
  if (ascending) {
#pragma omp parallel for
    for (unsigned i = 0; i < n - half_n; i++) {
      if (T[i] > T[i + half_n]) {
        int tmp = T[i];
        T[i] = T[i + half_n];
        T[i + half_n] = tmp;
      }
    }
  } else {
#pragma omp parallel for
    for (unsigned i = 0; i < n - half_n; i++) {
      if (T[i] < T[i + half_n]) {
        int tmp = T[i];
        T[i] = T[i + half_n];
        T[i + half_n] = tmp;
      }
    }
  }
// Recursive calls
#pragma omp parallel sections
  {
#pragma omp section
    { bitonic_sort_seq(T, half_n, ascending); }
#pragma omp section
    { bitonic_sort_seq(T + half_n, half_n, ascending); }
  }
}

void bitonic_sort_seq(int *T, unsigned n, bool ascending) {
  if (n == 1)
    return;

  unsigned half_n = n >> 1;
  // Recursive calls
  // Left half sorted ascending
  // Right half sorted descending
  bitonic_sort_seq(T, half_n, true);
  bitonic_sort_seq(T + half_n, half_n, false);

  // Bitonic merge
  bitonic_merge_seq(T, n, ascending);
}

void bitonic_sort(int *T, unsigned n, bool ascending) {
  if (n == 1)
    return;

  unsigned half_n = n >> 1;
// Recursive calls
// Left half sorted ascending
// Right half sorted descending
#pragma omp parallel sections
  {
#pragma omp section
    { bitonic_sort_seq(T, half_n, true); }
#pragma omp section
    { bitonic_sort_seq(T + half_n, half_n, false); }
  }

  // Bitonic merge
  bitonic_merge(T, n, ascending);
}
}
