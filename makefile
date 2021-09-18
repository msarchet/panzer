CC=g++
STD=-std=c++17
MAIN=./utils/board_utils.cpp ./sliding-attacks/sliders.cpp bitboard.cpp panzer-bitboard.cpp -lpthread
MAGIC=./utils/board_utils.cpp ./sliding-attacks/make-magic.cpp
TEST=./utils/board_utils.cpp ./sliding-attacks/sliders.cpp ./tests/slider_tests.cpp ./tests/check_tests.cpp ./bitboard.cpp ./tests/main.cpp -lpthread
DEBUG_OPTS=-ggdb -g3

main:
	$(CC) $(STD) $(MAIN) -O3 -o panzerbb

main-debug:
	$(CC) $(STD) $(MAIN) $(DEBUG_OPTS) -o panzerbb-debug

main-nonoptmized:
	$(CC) $(STD) $(MAIN) -o panzerbb-nonoptimized

magic:
	$(CC) $(STD) $(MAGIC) -O3 -o magic

magic-debug:
	$(CC) $(STD) $(MAGIC) $(DEBUG_OPTS) -o magic-debug

test:
	$(CC) $(STD) $(TEST) -o test

test-debug:
	$(CC) $(STD) $(TEST) $(DEBUG_OPTS) -o test-debug
