# Makefile for compiling solution.cpp

# Compiler and flags
CXX = g++
CXXFLAGS = -g -std=c++2a -Wall -Wextra -Werror -pedantic -O3

# Output executable name
TARGET = solution

# Source file
SRC = solution.cpp

# Default target
all: $(TARGET)

# Compile and link the executable
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -g -o $(TARGET) $(SRC)

# Clean up generated files
clean:
	rm -f $(TARGET)

run:
	./solution

.PHONY: all clean

