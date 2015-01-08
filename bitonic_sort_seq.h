/* Sequential bitonic sort */
void bitonic_sort_seq(int *T, unsigned n);

namespace bitonic_sort_seq_internal {
void bitonic_merge(int *T, unsigned n, bool ascending);

void bitonic_sort(int *T, unsigned n, bool ascending);
}
