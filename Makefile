all: program

program: csma_p.o node.o
	g++ -o csma_p csma_p.o node.o

main: csma_p.cpp
	g++ -c csma_p.cpp

node.o: node.cpp
	g++ -c node.cpp

clean:
	rm -f *.o *.txt *.png csma_p
