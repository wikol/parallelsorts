CC=g++
CFLAGS=-O2 -std=c++11 -fopenmp
LDFLAGS=
SOURCES := \
	main.cpp \
	bitonic_sort.cpp \
	radix_sort.cpp \
	merge_sort.cpp
EXECUTABLE=test_sort_algorithms

BIN := test_sort_algorithms

all:
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE)

clean:
	rm -rf test_sort_algorithms
