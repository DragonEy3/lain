CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra

OUT = lain.out

SRC = ./src/main.cpp
INC = ./src/*.h

all: $(OUT)

$(OUT): $(SRC) $(INC)
	$(CXX) $(SRC) -o $(OUT) $(CXXFLAGS)

clean:
	rm -f $(OUT)

.PHONY: all clean