compileP: CSMA_P.cpp node.cpp
	g++ -c node.cpp
	g++ -c CSMA_P.cpp
	g++ -o csma_p CSMA_P.o node.o

runP: csma_p

	./csma_p

plotP:	Efficiency_P_Graph.txt Throughput_P_Graph.txt
	gnuplot Efficiency_P_Graph.txt
	gnuplot Throughput_P_Graph.txt

cleanP:
	rm -f *.o E_P.txt TP_P.txt *.png csma_p

compileNP: CSMA_NP.cpp node.cpp
	g++ -c node.cpp
	g++ -c CSMA_NP.cpp
	g++ -o csma_np CSMA_NP.o node.o

runNP: csma_np
	./csma_np

plotNP:	Efficiency_NP_Graph.txt Throughput_NP_Graph.txt
	gnuplot Efficiency_NP_Graph.txt
	gnuplot Throughput_NP_Graph.txt

cleanNP:
	rm -f *.o E_NP.txt TP_NP.txt *.png csma_np
