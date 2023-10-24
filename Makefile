SRC := $(shell find . -name \*.cpp)
EXE := $(subst .cpp,.exe,$(SRC))

all: $(EXE)

%.exe: %.cpp
	g++ -Iinclude -std=c++20 -O0 -g -fsanitize=leak,address,undefined $< -o $@
	./$@
