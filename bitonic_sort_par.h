/* Parallel bitonic sort using OpenMP */
void bitonic_sort_par(int *T, unsigned n);

namespace bitonic_sort_par_internal {
void bitonic_merge_seq(int *T, unsigned n, bool ascending);

void bitonic_merge(int *T, unsigned n, bool ascending);

void bitonic_sort_seq(int *T, unsigned n, bool ascending);

void bitonic_sort(int *T, unsigned n, bool ascending);
}
