#include "merge_sort.h"

#include <cstring>
#include <algorithm>
#define PARALLEL_MERGE_THRESHOLD 16
#define PARALLEL_SORT_THRESHOLD 1024

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

void parallel_merge(int *first, unsigned first_size, int *second, unsigned second_size, int *dest) {
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

	// Recursive calls
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			parallel_merge(first, half_first_size, second, half_second_size, dest);
		}
		#pragma omp section
		{
			parallel_merge(first + half_first_size + 1, first_size - half_first_size - 1,
						   second + half_second_size, second_size - half_second_size,
						   dest + half_first_size + half_second_size + 1);
		}
	}
}

void hybrid_merge(int *first, unsigned first_size, int *second, unsigned second_size, int *dest) {
	// Make sure first is bigger
	if (first_size < second_size) {
		std::swap(first, second);
		std::swap(first_size, second_size);
	}

	// Nothing left to be merged
	if (first_size <= 0) 
		return;

	if (first_size + second_size <= PARALLEL_MERGE_THRESHOLD) {
		merge(first, first_size, second, second_size, dest);
		return;
	}

	unsigned half_first_size = first_size >> 1;
	int mid = first[half_first_size];
	unsigned half_second_size = find_element(second, second_size, mid);
	dest[half_first_size + half_second_size] = mid;

	// Recursive calls
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			hybrid_merge(first, half_first_size, second, half_second_size, dest);
		}
		#pragma omp section
		{
			hybrid_merge(first + half_first_size + 1, first_size - half_first_size - 1,
						 second + half_second_size, second_size - half_second_size,
						 dest + half_first_size + half_second_size + 1);
		}
	}
}

void merge_sort_par(int *T, unsigned n) {
	if (n <= 1)
		return;
	unsigned half_n = n >> 1;

	// Recursive calls
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			merge_sort_par(T, half_n);
		}
		#pragma omp section
		{
			merge_sort_par(T + half_n, n - half_n);
		}
	}

	int *buffer = new int[n];
	parallel_merge(T, half_n, T + half_n, n - half_n, buffer);
	memcpy(T, buffer, n * sizeof(int));
	delete[] buffer;
}

void merge_sort_seq(int *T, unsigned n) {
	if (n <= 1)
		return;
	unsigned half_n = n >> 1;

	merge_sort_seq(T, half_n);
	merge_sort_seq(T + half_n, n - half_n);

	int *buffer = new int[n];
	merge(T, half_n, T + half_n, n - half_n, buffer);
	memcpy(T, buffer, n * sizeof(int));
	delete[] buffer;
}

void merge_sort_par_merge_seq(int *T, unsigned n) {
	if (n <= 1)
		return;
	unsigned half_n = n >> 1;

	// Recursive calls
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			merge_sort_par_merge_seq(T, half_n);
		}
		#pragma omp section
		{
			merge_sort_par_merge_seq(T + half_n, n - half_n);
		}
	}

	int *buffer = new int[n];
	merge(T, half_n, T + half_n, n - half_n, buffer);
	memcpy(T, buffer, n * sizeof(int));
	delete[] buffer;
}

void merge_sort_hybrid(int *T, unsigned n) {
	if (n <= PARALLEL_SORT_THRESHOLD) {
		merge_sort_seq(T, n);
		return;
	}
	unsigned half_n = n >> 1;

	// Recursive calls
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			merge_sort_hybrid(T, half_n);
		}
		#pragma omp section
		{
			merge_sort_hybrid(T + half_n, n - half_n);
		}
	}

	int *buffer = new int[n];
	hybrid_merge(T, half_n, T + half_n, n - half_n, buffer);
	memcpy(T, buffer, n * sizeof(int));
	delete[] buffer;
}

} // namespace merge_sort_par::internal

void merge_sort_par(int *T, unsigned n) {
	merge_sort_internal::merge_sort_par(T, n);
}

void merge_sort_seq(int *T, unsigned n) {
	merge_sort_internal::merge_sort_seq(T, n);
}

void merge_sort_par_merge_seq(int *T, unsigned n) {
	merge_sort_internal::merge_sort_par_merge_seq(T, n);
}

void merge_sort_hybrid(int *T, unsigned n) {
	merge_sort_internal::merge_sort_hybrid(T, n);
}