CC=g++
STD=-std=c++17
MAIN=panzer-bitboard.cpp

debug:
	$(CC) $(STD) $(MAIN) -ggdb -g3 -o panzerbb-debug

nonoptmized:
	$(CC) $(STD) $(MAIN) -o panzerbb-nonoptimized

optimized:
	$(CC) $(STD) $(MAIN) -O3 -o panzerbb

