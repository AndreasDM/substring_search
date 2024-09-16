all:
	@g++ main.cpp -O3 -std=c++20 -lbenchmark

run: all
	@./a.out
