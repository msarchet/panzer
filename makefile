CC=g++
STD=-std=c++17
INCLUDE=-I ./include
MAIN=./src/board_utils.cpp ./sliding-attacks/sliders.cpp ./src/bitboard.cpp ./src/panzer-bitboard.cpp -lpthread
MAGIC=./utils/board_utils.cpp ./sliding-attacks/make-magic.cpp
TEST=./utils/board_utils.cpp ./sliding-attacks/sliders.cpp ./tests/slider_tests.cpp ./tests/check_tests.cpp ./bitboard.cpp ./tests/main.cpp -lpthread
DEBUG_OPTS=-ggdb -g3

main:
	$(CC) $(STD) $(INCLUDE) $(MAIN) -Wall -O3 -o panzerbb

main-debug:
	$(CC) $(STD) $(INCLUDE) $(MAIN) $(DEBUG_OPTS) -o panzerbb-debug

main-nonoptmized:
	$(CC) $(STD) $(INCLUDE) $(MAIN) -o panzerbb-nonoptimized

magic:
	$(CC) $(STD) $(INCLUDE) $(MAGIC) -O3 -o magic

magic-debug:
	$(CC) $(STD) $(INCLUDE) $(MAGIC) $(DEBUG_OPTS) -o magic-debug

test:
	$(CC) $(STD) $(INCLUDE) $(TEST) -o test

test-debug:
	$(CC) $(STD) $(INCLUDE) $(TEST) $(DEBUG_OPTS) -o test-debug
