#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <map>
#include "node.h"

using namespace std;

//Finds the index of the node with the lowest arrival time
int nextTransmitter(vector<node> LAN) {
  int min = 0;
  bool empty = LAN[min].isEmpty();
  for (int i = 1; i < LAN.size(); i++) {
    if (!LAN[i].isEmpty()) {
      empty = false;
      if (LAN[min].isEmpty() || LAN[i].next() < LAN[min].next()) {
        min = i;
      }
    }
  }
  if (empty) {
    return -1;
  }
  return min;
}

int main(int argc, char* argv[]) {
  int initTime = (unsigned) time(nullptr);
  srand(initTime);  //Initialize seed for pseudorandom uniform generator
  int testTime = initTime;

  ofstream outputE;
  ofstream outputTP;
  outputE.open("E_P.txt");
  outputTP.open("TP_P.txt");

  // int N = stoi(argv[1]);    //Number of nodes
  int N[5] = {20, 40, 60, 80, 100};  //Number of nodes
  float A[3] = {7, 10, 20};           //Average arrival rate (packets/s)
  int T = 200;             //Simulation time (s)
  const int R = 1000000;             //Channel speed (bits/s)
  const int L = 1500;                //Packet length (bits)
  const int D = 10;                  //Distance between adjacent nodes (m)
  const int S = 200000000;           //Propagation speed (m/s)

  double propTime = (double) D/S;
  double transTime = (double) L/R;

  vector<node> LAN;

  vector<int> collisions;  //Indices of all nodes involved in a collision

  for (int j = 0; j < 5; j++) { //Loop through N range

    outputE << N[j];
    outputTP << N[j];

    for (int m = 0; m < 3; m++) { //Loop through range of arrival rates

      int numAttempts = 0;
      int numSuccesses = 0;
      int numDropped = 0;

      //Populate the queues of all nodes until T seconds.

      double propTimeIndex[N[j]];
      for (int i = 0; i < N[j]; i++) {
        LAN.push_back(node(A[m], T, R));
        propTimeIndex[i] = (double) i * propTime; //Functions accessing an element propTimeIndex[j] are asking for the propagation delay for a distance 10*j meters
      }

      // for (int i = 0; i < N[j]; i++) {  //To save time and not do astronomical amounts of double-precision floating point computations
      //   propTimeIndex[i] = (double) i * propTime; //Functions accessing an element propTimeIndex[j] are asking for the propagation delay for a distance 10*j meters
      // }

      int transmitter = nextTransmitter(LAN); //The index of the node trying to send

      while (LAN[transmitter].next() < T) {
        numAttempts++;  //The sending node attempts to transmit
        int maxDist = 0;
        for (int i = 0; i < LAN.size(); i++) {
          int distance = abs(transmitter - i);
          if (i != transmitter && LAN[i].next() <= (LAN[transmitter].next() + propTimeIndex[distance])) {
            numAttempts++;
            collisions.push_back(i); //Adding to the list of nodes that "transmitter" collides with for this frame
            LAN[i].collide();
            maxDist = max(maxDist, distance);
          }
        }

        if (collisions.empty()) {
          for (int i = 0; i < LAN.size(); i++) {
            int distance = abs(transmitter - i);
            double delay = LAN[transmitter].next() + propTimeIndex[distance] + transTime;
            if (delay < T && LAN[i].next() <= delay) {
              LAN[i].wait(delay); //Nodes (including the sender!) must wait until the entire packet has passed.
            }
          }
          LAN[transmitter].send();
          numSuccesses++;
        } else {
          LAN[transmitter].collide();
          collisions.push_back(transmitter);
          for (int i = 0; i < collisions.size(); i++) {
            if (!LAN[collisions[i]].backOff(propTimeIndex[maxDist] + LAN[transmitter].next(), T)) {
              ++numDropped;
              //STRONG ASSUMPTION: backoff for each node in the collision is based off the maximum propagation time
              //involved in the collision.
            }
          }
        }
        collisions.clear();
        transmitter = nextTransmitter(LAN);  //Find the attempting transmitter for the next loop
      }

      float efficiency = (float) numSuccesses / (numAttempts);
      float throughput = (float) numSuccesses * L / T;
      outputE << " " << efficiency;
      outputTP << " " << (float)throughput/R;

      cout << "For N = " << N[j] << ", A = " << A[m] << endl;
      /*cout << "Attempts: " << numAttempts << endl;
      cout << "Successes: " << numSuccesses << endl;
      cout << "Dropped: " << numDropped << endl;*/
      cout << "Efficiency: " << efficiency << endl;
      cout << "Throughput: " << throughput/R << endl;
      cout << "Step took " << (unsigned)time(nullptr) - testTime << " seconds." << endl << endl;
      testTime = (unsigned)time(nullptr);

      LAN.clear();

    }

    outputE << "\n";
    outputTP << "\n";

  }

  outputE.close();
  outputTP.close();

  cout << "Finished. Runtime is " << ((unsigned)time(nullptr) - initTime)/60 << " minutes." << endl;
}
