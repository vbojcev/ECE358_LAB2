#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include "node.h"

using namespace std;

//Finds the index of the node with the lowest arrival time
int nextTransmitter(vector<node> &LAN) {
  int min = 0;
  for (int i = 1; i < LAN.size(); i++) {
    if (LAN[i].next() < LAN[min].next()) {
      min = i;
    }
  }
  return min;
  /* This logic does give preference to the lowered number node
  in the case that two times are equal, however since the nodes
  are not unique and the timescale is large this is insignificant. */
}

int main(int argc, char* argv[]) {

  //Initialize pseudorandom generator using time
  int initTime = (unsigned) time(nullptr);
  srand(initTime);
  int testTime = initTime;

  //Initialize file outputs
  ofstream outputE;
  ofstream outputTP;
  outputE.open("E_NP.txt");
  outputTP.open("TP_NP.txt");

  int N[5] = {20, 40, 60, 80, 100}; //Number of nodes
  float A[3] = {7, 10, 20};         //Average arrival rate (packets/s)
  int T = 250;                      //Simulation time (s)
  const int R = 1000000;            //Channel speed (bits/s)
  const int L = 1500;               //Packet length (bits)
  const int D = 10;                 //Distance between adjacent nodes (m)
  const int S = 200000000;          //Propagation speed (m/s)

  double propTime = (double) D/S;   //Constant adjecent prop. delay
  double transTime = (double) L/R;  //Constant transmission delay

  double next;                      //Packet attempting to be sent

  vector<node> LAN;                 //Vector of all the nodes using the channel

  double propTimeIndex[100];

  for (int i = 0; i < 100; i++) {
    propTimeIndex[i] = (double) i * propTime;
    //In an attempt to save time, we pre-calculate all possible propagation delays between nodes.
  }

  for (int j = 0; j < 5; j++) { //Loop through N range

    outputE << N[j];
    outputTP << N[j];

    for (int m = 0; m < 3; m++) { //Loop through range of arrival rates, A

      int numAttempts = 0;
      int numSuccesses = 0;
      int numDropped = 0;
      int numDroppedNP = 0;

      //Populate the queues of all nodes until T seconds.
      for (int i = 0; i < N[j]; i++) {
        LAN.push_back(node(A[m], T, R));
        //This is done in the same way as in Lab 1, however now we are allowed to use queues.
      }

      int transmitter = nextTransmitter(LAN); //The index of the node trying to send

      while ((next = LAN[transmitter].next()) < T) {
        numAttempts++;  //The sending node attempts to transmit, this is always incremented
        LAN[transmitter].resetNP(); //The sending node sees the channel vacant, reset NP counter.

        int maxDist = 0;
        for (int i = 0; i < LAN.size(); i++) {
          int distance = abs(transmitter - i);
          //There IS a collision:
          if (i != transmitter && LAN[i].next() <= (next + propTimeIndex[distance])) {
            numAttempts++;  //Collision implies packet is on the channel, therefore counts as an attempt.
            if (!LAN[i].backOff(propTimeIndex[distance] + next)) {  //Backoff, but also check if the packet is dropped.
              ++numDropped;
            }
            maxDist = max(maxDist, distance);
          }
        }

        //There IS NOT a collision:
        if (!maxDist) {
          LAN[transmitter].wait(next + transTime);
          LAN[transmitter].send();
          numSuccesses++;

          for (int i = 0; i < LAN.size(); i++) {
            if (i != transmitter) {
              if (!LAN[i].npBackOff(next + propTimeIndex[abs(transmitter - i)] + transTime)) {
                ++numDroppedNP;
              }
            }
          }
          
        } else {
          //Transmitter's collision backoff is calculated separately.
          //This is because we can't know the farthest node (maxDist) without looping through all other nodes first
          if (!LAN[transmitter].backOff(propTimeIndex[maxDist] + next)) {
            ++numDropped;
          }
        }
        transmitter = nextTransmitter(LAN);  //Find the attempting transmitter for the next loop
      }

      //Calculate measurements, output to file
      float efficiency = (float) numSuccesses / (numAttempts + numDroppedNP);
      float throughput = (float) numSuccesses * L / T;
      outputE << " " << efficiency;
      outputTP << " " << (float)throughput/R;

      //Display during runtime so we can monitor results and execution speed
      cout << "For N = " << N[j] << ", A = " << A[m] << endl;
      cout << "Dropped due to busy = " << numDroppedNP << endl;
      cout << "Number of Collisions = " << numAttempts - numSuccesses << endl;
      cout << "Efficiency: " << efficiency << endl;
      cout << "Throughput: " << throughput/R << endl;
      cout << "Step took " << (unsigned)time(nullptr) - testTime << " seconds." << endl << endl;
      testTime = (unsigned)time(nullptr); //testTime is the starting point of the next loop

      LAN.clear();
    }

    outputE << "\n";
    outputTP << "\n";

  }

  outputE.close();
  outputTP.close();

  cout << "Finished. Runtime is " << ((unsigned)time(nullptr) - initTime)/60 << " minutes and " << ((unsigned)time(nullptr) - initTime)%60 << " seconds." << endl;
}
