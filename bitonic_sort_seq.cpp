#include "bitonic_sort_seq.h"

namespace bitonic_sort_seq_internal {

void bitonic_sort(int *T, unsigned n, bool ascending);

void bitonic_merge(int *T, unsigned n, bool ascending) {
  if (n == 1)
    return;

  unsigned half_n = n >> 1;
  if (ascending) {
    for (unsigned i = 0; i + half_n < n; i++) {
      if (T[i] > T[i + half_n]) {
        int tmp = T[i];
        T[i] = T[i + half_n];
        T[i + half_n] = tmp;
      }
    }
  } else {
    for (unsigned i = 0; i + half_n < n; i++) {
      if (T[i] < T[i + half_n]) {
        int tmp = T[i];
        T[i] = T[i + half_n];
        T[i + half_n] = tmp;
      }
    }
  }
  // Recursive calls
  bitonic_sort(T, half_n, ascending);
  bitonic_sort(T + half_n, half_n, ascending);
}

void bitonic_sort(int *T, unsigned n, bool ascending) {
  if (n == 1)
    return;

  unsigned half_n = n >> 1;
  // Recursive calls
  // Left half sorted ascending
  // Right half sorted descending
  bitonic_sort(T, half_n, true);
  bitonic_sort(T + half_n, half_n, false);

  // Bitonic merge
  bitonic_merge(T, n, ascending);
}
}

/* Sequential bitonic sort */
void bitonic_sort_seq(int *T, unsigned n) {
  bitonic_sort_seq_internal::bitonic_sort(T, n, true);
}
