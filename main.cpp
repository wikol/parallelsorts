#include <omp.h>

#include <cstdio>
#include <functional>
#include <algorithm>
#include <cassert>
#include <random>
#include <cstring>
#include <limits>
#include <chrono>

#include "bitonic_sort.h"
#include "merge_sort.h"
#include "radix_sort.h"

void test_one_algorithm(std::function<void(int*, unsigned)> sort_to_test,
						int *data, unsigned size, const char *message) {
	std::printf("\t%s\n", message);

	int *data_copy = new int[size];
	std::memcpy(data_copy, data, size*sizeof(int));

	auto start = std::chrono::high_resolution_clock::now();
	sort_to_test(data_copy, size);
    auto finish = std::chrono::high_resolution_clock::now();
    std::printf("\t\tReal time used: %lums\n",
    			std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count());

	int *data_copy_for_std = new int[size];
	std::memcpy(data_copy_for_std, data, size*sizeof(int));
	std::sort(data_copy_for_std, data_copy_for_std + size);

	if (std::memcmp(data_copy, data_copy_for_std, size*sizeof(int)) != 0) {
		std::printf("\t\tIncorrect output!\n");
		std::exit(1);
	} else {
		std::printf("\t\tThe output was correct.\n");
	}

	delete[] data_copy;
	delete[] data_copy_for_std;
}

void std_sort(int *T, unsigned n) {
	std::sort(T, T+n);
}

void test_all(int *data, unsigned size, const char *message) {
	std::printf("%s\n", message);

	// Add new sorting algorithms here
	test_one_algorithm(std_sort, data, size, "Running std::sort");
  test_one_algorithm(bitonic_sort_seq, data, size, "Running sequential bitonic sort");
  test_one_algorithm(bitonic_sort_par, data, size, "Running parallel bitonic sort");
	test_one_algorithm(merge_sort_seq, data, size, "Running sequential mergesort");
	test_one_algorithm(merge_sort_par, data, size, "Running parallel mergesort");
	test_one_algorithm(merge_sort_par_merge_seq, data, size,
					   "Running parallel mergesort with sequential merges");
	test_one_algorithm(merge_sort_hybrid, data, size, "Running hybrid mergesort");
  test_one_algorithm(radix_sort_seq, data, size, "Running sequential radixsort");
  test_one_algorithm(radix_sort_par, data, size, "Running parallel radixsort");
}

void test_on_random_data(unsigned size) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());

	int *data = new int[size];
	for (unsigned i = 0;i<size;i++)
		data[i] = dis(gen);

	char message[100];
	std::sprintf(message, "Testing on random data of size %u", size);

	test_all(data, size, message);
}

const int TESTCASE_SIZE = 1 << 24;

int main(int argc, char **argv) {
  omp_set_nested(true);
  if(argc > 1) {
    omp_set_num_threads(std::atoi(argv[1]));
  }
	test_on_random_data(TESTCASE_SIZE);
	return 0;
}
