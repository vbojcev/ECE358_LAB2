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

  cout << propTime << endl;

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
    // map<int, double> collisions;  //Indices of all nodes involved in a collision
    
    int transmitter = nextTransmitter(LAN); //The index of the node trying to send

    while (LAN[transmitter].next() < T) {
      numAttempts++;
      // cout << transmitter << ": " << LAN[transmitter].next() << endl;

      //Check for collisions
      int maxDist;
      for (int i = 0; i < LAN.size(); i++) {
        int distance = abs(transmitter - i);
        if (distance > 0 && !LAN[i].isEmpty() && LAN[i].next() <= (LAN[transmitter].next() + distance*propTime)) {
          //numAttempts++;
          collisions.push_back(i);
          LAN[i].collide();
          maxDist = max(maxDist, distance);
          //cout << "Node " << transmitter << " collides with " << i << ". " << LAN[transmitter].next() << " " << LAN[i].next() << endl;
        }
      }

      if (collisions.empty()) {
        //cout << "Node " << transmitter << " Successful" << endl;
        for (int i = 0; i < LAN.size(); i++) {
          int distance = abs(transmitter - i);
          if (!LAN[i].isEmpty() &&/* LAN[i].next() > LAN[transmitter].next() + distance*propTime &&*/ LAN[i].next() < LAN[transmitter].next() + distance*propTime + transTime) {
          // if (!LAN[i].isEmpty() && LAN[i].next() < LAN[transmitter].next() + distance*propTime + transTime) {
            LAN[i].wait(LAN[transmitter].next() + distance*propTime + transTime);
          }
        }
        LAN[transmitter].send();
        numSuccesses++;
      } else {
        LAN[transmitter].collide();
        LAN[transmitter].backOff((double)maxDist * propTime);
        for (int i = 0; i < collisions.size() - 1; i++) {
          if (!LAN[collisions[i]].backOff((double)abs(i - transmitter)*propTime)) {
            numDropped++;
          }
        }
      }

      collisions.clear();
      transmitter = nextTransmitter(LAN);  //Find the attempting transmitter for the next loop
    }

    double efficiency = numSuccesses / (double) numAttempts;
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
