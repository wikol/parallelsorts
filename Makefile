
SOURCE := \
	main.cpp \
	bitonic_sort_par.cpp \
	bitonic_sort_seq.cpp \
	radix_sort_par.cpp \
	radix_sort_seq.cpp \
	merge_sort.cpp

BIN := test_sort_algorithms

all:
	g++ -std=c++11 -fopenmp -o $(BIN) $(SOURCE)
