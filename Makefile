C_FILES=main.cpp
H_FILES=functors.hpp puzzle_state.hpp
STD=-std=c++11
O_FLAGS=-fno-omit-frame-pointer
R_FLAGS=-Ofast -s -Wall
D_FLAGS=-g -Wall

all: eight_gcc eight_gcc_d eight_clang eight_clang_d

rebuild:
	make clean
	make
	ls -lash

clean:
	rm eight_gcc eight_gcc_d eight_clang eight_clang_d

eight_gcc: $(C_FILES) $(H_FILES)
	g++ $(C_FILES) $(STD) $(R_FLAGS) -o eight_gcc

eight_gcc_d: $(C_FILES) $(H_FILES)
	g++ $(C_FILES) $(STD) $(D_FLAGS) -o eight_gcc_d

eight_clang: $(C_FILES) $(H_FILES)
	clang++ $(C_FILES) $(STD) $(R_FLAGS) -o eight_clang

eight_clang_d: $(C_FILES) $(H_FILES)
	clang++ $(C_FILES) $(STD) $(D_FLAGS) -o eight_clang_d

