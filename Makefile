CXXFLAGS=-O3
CFLAGS=-O3

all: compress uncompress
compress: main.o compression.o
	c++ main.o compression.o -o compress
uncompress: main.o compression.o
	c++ main.o compression.o -o uncompress
main.o: main.cpp compression.h
compression.o: compression.cpp compression.h
clean:
	rm -f *.o compress uncompress
