/* Parallel merge sort using OpenMP */
void merge_sort_par(int *T, unsigned n);

/* Sequential merge sort */
void merge_sort_seq(int *T, unsigned n);

/* Parallel merge sort, which uses sequential merge */
void merge_sort_par_merge_seq(int *T, unsigned n);

/* Merge sort, which combines sequential and parallel approaches */
void merge_sort_hybrid(int *T, unsigned n);