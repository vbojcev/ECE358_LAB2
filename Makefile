compileP: CSMA_P.cpp node.cpp
	g++ -c node.cpp
	g++ -c CSMA_P.cpp
	g++ -o csma_p CSMA_P.o node.o

runP: csma_p

	./csma_p 5 300

plotP:	graph
	gnuplot graph

cleanP: 
	rm -f *.o *.txt *.png csma_p
