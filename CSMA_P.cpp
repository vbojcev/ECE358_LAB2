#include <iostream>  //For debugging
#include <cmath>  //For logarithm
#include <vector>  //Used only as variable-size array (not explicit queue)
#include <algorithm>
#include <time.h>  //Used to set the seed of the random number generator
#include <fstream>  //Used to output to a file
#include <climits>
#include <queue>

using namespace std;

int N;  //Number of nodes
float A;  //Average arrival rate for each node's buffer
int T;  //Simulation time
int R;  //Speed, in bit/s
int L;  //Packet lengths, in bits
int D;  //Distance between nodes on channel, in metres
int S;  //Propagation speed, in m/s

float expVar(float rate) {  //Exponentially-distributed value generator; exactly the same as LAB1's.
  return (float)-(1/rate)*log(1-((float)(rand()%1000)/1000));
}

class node {

  private:

    int i = 0;  //Backoff counter
    dequeue<float> frameQueue;  //The frame queue only needs to be a queue of timestamps
    int k = 0;  //Iterator variable


  public:

    node(int popTime, int arriveRate) { //This is the only way that a node can be constructed.

       float currTime = 0;

       while (currTime < popTime) {
         currTime += expVar(arriveRate);
         frameQueue.push_back(currTime);
       }
    }

    int next() {
      if (frameQueue.empty()) {
        return T + 10;  //A value much higher than any reasonable simulation time
      } else {
        return frameQueue.front();
      }
    }

    void deQueue() {
      frameQueue.pop_front();
      i = 0;
      }

    bool isEmpty() {return frameQueue.empty();}

    bool backoff(float propTime) {
      ++i;

      if (i > 10) {
        frameQueue.pop_front();
        i = 0;
        return false;
      } else {
        k = 0;
        frameQueue[0] = (rand() % (pow(2, i)-2) + 1)/R + propTime; //Returns random number in [1,(2^i)-1], inclusive
        ++k;
        while (frameQueue[k] <= frameQueue[0]) {
          frameQueue[k] = frameQueue[0];
        }
        return true;
      }

    }

};


int findMin(vector<node> network) { //This function finds the index of the node with the next smallest arrival time
  int min = 0;

  for (int i = 0; i < network.size(); ++i) {
    if (network[i].next() < network[min].next()) {
      min = i;
    }
  }
  return min;
}

int main(int argc, char* argv[]) {

  int initTime = (unsigned)time(nullptr);

  srand(initTime);  //Initialize seed for pseudorandom uniform generator

  N = stoi(argv[1]);  //Number of nodes
  A = stof(argv[2]);  //Average arrival rate for each node's buffer
  T = stoi(argv[3]);
  R = 1000000;  //Speed, in bit/s
  L = 1500;  //Packet lengths, in bits
  D = 10;  //Distance between nodes on channel, in metres
  S = 200000000;  //Propagation speed, in m/s

  int numAttempts = 0;
  int numSuccesses = 0;

  vector<node> LAN;

  for (int i = 0; i < N; i++) {  //Pre-simulation, populate the queues of all nodes until T seconds.
    LAN.push_back(node(T, A));
  }

  float simTime = 0;

  int sender = findMin(LAN); //The index of the node trying to send

  while ((LAN[sender].next()) < T) {


    sender = findMin(LAN);
  }

}
