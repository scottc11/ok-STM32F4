# Makefile to compile and execute a single C++ file

# Compiler
CC = g++

# Compiler flags
CFLAGS = -std=c++11 -Wall

# Source file
SRC = playground.cpp

# build directory
BUILD_DIR = build

# Executable name
EXEC = playground

all: compile run

compile: $(SRC)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(EXEC) $(SRC)

run: $(BUILD_DIR)/$(EXEC)
	./$(BUILD_DIR)/$(EXEC)

clean:
	rm -fR $(BUILD_DIR)