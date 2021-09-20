CC=g++
STD=-std=c++17
INCLUDE=-I ./include
MAIN=./src/board_utils.cpp ./sliding-attacks/sliders.cpp ./src/bitboard.cpp ./src/panzer-bitboard.cpp
MAGIC=./utils/board_utils.cpp ./sliding-attacks/make-magic.cpp
TEST=./utils/board_utils.cpp ./sliding-attacks/sliders.cpp ./tests/slider_tests.cpp ./tests/check_tests.cpp ./bitboard.cpp ./tests/main.cpp
DEBUG_OPTS=-ggdb -g3

main:
	$(CC) $(STD) $(INCLUDE) $(MAIN) -lpthread -Wall -O3 -o panzer

main-debug:
	$(CC) $(STD) $(INCLUDE) $(MAIN) $(DEBUG_OPTS) -lpthread -Wall -o panzer-debug

main-nonoptmized:
	$(CC) $(STD) $(INCLUDE) $(MAIN) -Wall -lpthread -o panzer-nonoptimized

magic:
	$(CC) $(STD) $(INCLUDE) $(MAGIC) -O3 -Wall -o magic

magic-debug:
	$(CC) $(STD) $(INCLUDE) $(MAGIC) $(DEBUG_OPTS) -Wall -o magic-debug

test:
	$(CC) $(STD) $(INCLUDE) $(TEST) -Wall -lpthreadd -o test

test-debug:
	$(CC) $(STD) $(INCLUDE) $(TEST) $(DEBUG_OPTS) -Wall -lpthread -o test-debug
