#ifndef __BITONICSORT_H__
#define __BITONICSORT_H__

/* Sequential bitonic sort */
void bitonic_sort_seq(int *T, unsigned n);

/* Parallel bitonic sort using OpenMP */
void bitonic_sort_par(int *T, unsigned n);

#endif
