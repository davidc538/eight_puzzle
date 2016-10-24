C_FILES=main.cpp
STD=-std=c++11
R_FLAGS=-O2
D_FLAGS=-g

all: eight_gcc eight_gcc_d eight_clang eight_clang_d

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

