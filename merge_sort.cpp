#include "merge_sort.h"

#include <cstring>
#include <algorithm>
#include <omp.h>

namespace merge_sort_internal {

void merge(int *first, unsigned first_size, int *second, unsigned second_size, int *dest) {
	while (first_size > 0 && second_size > 0) {
		if (*first < *second) {
			*dest = *first;
			first++;
			first_size--;
		} else {
			*dest = *second;
			second++;
			second_size--;
		}
		dest++;
	}
	std::memcpy(dest, first, first_size*sizeof(int));
	std::memcpy(dest, second, second_size*sizeof(int));
}

int find_element(int *T, unsigned n, int element) {
	int low = 0, high = n;
	while (low < high) {
		int mid = (low + high) >> 1;
		if (T[mid] < element)
			low = mid + 1;
		else
			high = mid;
	}
	return low;
}

void parallel_merge(int *first, unsigned first_size, int *second, unsigned second_size, int *dest, unsigned threads) {
	if (threads <= 1) {
		merge(first, first_size, second, second_size, dest);
		return;
	}
	// Make sure first is bigger
	if (first_size < second_size) {
		std::swap(first, second);
		std::swap(first_size, second_size);
	}

	// Nothing left to be merged
	if (first_size <= 0) 
		return;

	unsigned half_first_size = first_size >> 1;
	int mid = first[half_first_size];
	unsigned half_second_size = find_element(second, second_size, mid);
	dest[half_first_size + half_second_size] = mid;
	unsigned half_threads = threads >> 1;


	// Recursive calls
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			parallel_merge(first, half_first_size, second, half_second_size, dest, half_threads);
		}
		#pragma omp section
		{
			parallel_merge(first + half_first_size + 1, first_size - half_first_size - 1,
						   second + half_second_size, second_size - half_second_size,
						   dest + half_first_size + half_second_size + 1, threads - half_threads);
		}
	}
}

void merge_sort_seq(int *T, unsigned n, int *buffer) {
	if (n <= 1)
		return;
	unsigned half_n = n >> 1;

	merge_sort_seq(T, half_n, buffer);
	merge_sort_seq(T + half_n, n - half_n, buffer + half_n);

	merge(T, half_n, T + half_n, n - half_n, buffer);
	memcpy(T, buffer, n * sizeof(int));
}

void merge_sort_par(int *T, unsigned n, int *buffer, unsigned threads) {
	if (n <= 1)
		return;
	if (threads <= 1) {
		merge_sort_seq(T, n, buffer);
		return;
	}
	unsigned half_n = n >> 1;
	unsigned half_threads = threads >> 1;
	// Recursive calls
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			merge_sort_par(T, half_n, buffer, half_threads);
		}
		#pragma omp section
		{
			merge_sort_par(T + half_n, n - half_n, buffer + half_n, threads - half_threads);
		}
	}

	parallel_merge(T, half_n, T + half_n, n - half_n, buffer, threads);
	memcpy(T, buffer, n * sizeof(int));
}

void merge_sort_par_merge_seq(int *T, unsigned n, int *buffer, unsigned threads) {
	if (n <= 1)
		return;
	if (threads <= 1) {
		merge_sort_seq(T, n, buffer);
		return;
	}

	unsigned half_n = n >> 1;
	unsigned half_threads = threads >> 1;

	// Recursive calls
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			merge_sort_par_merge_seq(T, half_n, buffer, half_threads);
		}
		#pragma omp section
		{
			merge_sort_par_merge_seq(T + half_n, n - half_n, buffer + half_n, threads - half_threads);
		}
	}

	merge(T, half_n, T + half_n, n - half_n, buffer);
	memcpy(T, buffer, n * sizeof(int));
}

} // namespace merge_sort_par::internal

void merge_sort_par(int *T, unsigned n) {
	int *buffer = new int[n];
	merge_sort_internal::merge_sort_par(T, n, buffer, omp_get_max_threads());
	delete[] buffer;
}

void merge_sort_seq(int *T, unsigned n) {
	int *buffer = new int[n];
	merge_sort_internal::merge_sort_seq(T, n, buffer);
	delete[] buffer;
}

void merge_sort_par_merge_seq(int *T, unsigned n) {
	int *buffer = new int[n];
	merge_sort_internal::merge_sort_par_merge_seq(T, n, buffer, omp_get_max_threads());
	delete[] buffer;
}