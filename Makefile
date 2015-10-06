CXX=g++
CXXFLAGS=-std=c++11

.PHONY: all clean

all: mountgraph

mountgraph: mountgraph.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f mountgraph
