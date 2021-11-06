compileP: CSMA_P.cpp node.cpp
	g++ -c node.cpp
	g++ -c CSMA_P.cpp
	g++ -o csma_p CSMA_P.o node.o

runP: csma_p

	./csma_p

plotP:	Efficiency_P_Graph.txt Throughput_P_Graph.txt
	gnuplot Efficiency_P_Graph.txt
	gnuplot Throughput_P_Graph.txt_

cleanP: 
	rm -f *.o E_P.txt TP_P.txt *.png csma_p
