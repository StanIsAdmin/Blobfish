
default: all

all:
	mkdir -p bin
	g++ -Wall -std=c++11 src/main.cpp -o bin/blobfish

debug:
	mkdir -p bin
	g++ -Wall -std=c++11 -g src/main.cpp -o bin/blobfish
	
clean:
	rm bin/blobfish
