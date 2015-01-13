
SOURCE := \
	main.cpp \
	bitonic_sort.cpp \
	radix_sort.cpp \
	merge_sort.cpp

BIN := test_sort_algorithms

all:
	g++ -std=c++11 -fopenmp -o $(BIN) $(SOURCE)
