# Compiler
CXX = mpicxx

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -Wextra -I./src

# Targets
.PHONY: test clean

# Test target
test: test/test.cpp src/mpi_utils.hpp
	$(CXX) $(CXXFLAGS) -o test_runner test/test.cpp
	mpirun -np 4 ./test_runner

# Clean target
clean:
	rm -f test_runner
