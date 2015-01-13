CC=g++
CFLAGS=-O2 -std=c++11 -fopenmp
LDFLAGS=
SOURCES := \
	main.cpp \
	bitonic_sort.cpp \
	radix_sort.cpp \
	merge_sort.cpp

BIN := test_sort_algorithms

all:
	$(CC) $(CFLAGS) $(SOURCES) -o $(BIN)

clean:
	rm -rf test_sort_algorithms
