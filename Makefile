CXXFLAGS=-O0 -g
CFLAGS=-O0 -g

all: compress uncompress
compress: main.o compression.o
	c++ main.o compression.o -o compress -g
uncompress: main.o compression.o
	c++ main.o compression.o -o uncompress -g
main.o: main.cpp compression.h
compression.o: compression.cpp compression.h
clean:
	rm -f *.o compress uncompress
