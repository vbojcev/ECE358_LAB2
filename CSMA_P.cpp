#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <map>
#include "node.h"


//TEST

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

  ofstream output;
  output.open("data.txt");

  // int N = stoi(argv[1]);    //Number of nodes
  int N[5] = {20, 40, 60, 80, 100};  //Number of nodes
  float A = stof(argv[1]);           //Average arrival rate (packets/s)
  int T = stoi(argv[2]);             //Simulation time (s)
  const int R = 1000000;             //Channel speed (bits/s)
  const int L = 1500;                //Packet length (bits)
  const int D = 10;                  //Distance between adjacent nodes (m)
  const int S = 200000000;           //Propagation speed (m/s)

  double propTime = (double) D/S;
  double transTime = (double) L/R;

  cout << "hello3" << endl;

  vector<node> LAN;

  vector<int> collisions;  //Indices of all nodes involved in a collision

  for (int j = 0; j < 5; j++) {
    int numAttempts = 0;
    int numSuccesses = 0;
    int numDropped = 0;

    //Populate the queues of all nodes until T seconds.
    
    for (int i = 0; i < N[j]; i++) {
      LAN.push_back(node(A, T, R));
    }

    double propTimeIndex[N[j]];

    for (int i = 0; i < N[j]; ++i) {  //To save time and not do astronomical amounts of double-precision floating point computations
      propTimeIndex[i] = (double) i * propTime; //Functions accessing an element propTimeIndex[j] are asking for the propagation delay for a distance 10*j meters
    }

    // map<int, double> collisions;  //Indices of all nodes involved in a collision
    
    int transmitter = nextTransmitter(LAN); //The index of the node trying to send

    while (LAN[transmitter].next() < T) {
      numAttempts++;
      int maxDist = 0;
      for (int i = 0; i < LAN.size(); i++) {
        int distance = abs(transmitter - i);
        if (i != transmitter && LAN[i].next() <= (LAN[transmitter].next() + propTimeIndex[distance])) {
          numAttempts++;
          collisions.push_back(i);
          LAN[i].collide();
          if (!LAN[i].backOff(propTimeIndex[distance] + LAN[transmitter].next())){
            ++numDropped;
          }
          maxDist = max(maxDist, distance);
        }
      }

      if (collisions.empty()) {
        for (int i = 0; i < LAN.size(); i++) {
          int distance = abs(transmitter - i);
          if (i != transmitter && LAN[i].next() < LAN[transmitter].next() + propTimeIndex[distance] + transTime) {
            LAN[i].wait(LAN[transmitter].next() + propTimeIndex[distance] + transTime);
          }
        }
        LAN[transmitter].send();
        numSuccesses++;
      } else {
        LAN[transmitter].collide();
        if (!LAN[transmitter].backOff(propTimeIndex[maxDist] + LAN[transmitter].next())) {
          ++numDropped;
        }
      }

      collisions.clear();
      transmitter = nextTransmitter(LAN);  //Find the attempting transmitter for the next loop
    }

    float efficiency = (float) numSuccesses / (numAttempts + numDropped) ;
    output << N[j] << " " << efficiency << "\n";

    cout << "Attempts: " << numAttempts << endl;
    cout << "Successes: " << numSuccesses << endl;
    cout << "Dropped: " << numDropped << endl;
    cout << "Efficiency: " << efficiency << endl << endl;

    LAN.clear();
  }

  output.close();

  cout << "Finished. Runtime is " << ((unsigned)time(nullptr) - initTime)/60 << " minutes." << endl;
}
