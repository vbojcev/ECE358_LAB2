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

double expVar(float rate) {  //Exponentially-distributed value generator; exactly the same as LAB1's.
  return (double)-(1/rate)*log(1-((double)(rand()%1000)/1000));
}

class node {

  private:

    int i = 0;  //Backoff counter
    dequeue<double> frameQueue;  //The frame queue only needs to be a queue of timestamps, unlike LAB1 where we had event objects
    int k = 0;  //Iterator variable


  public:

    node(int popTime, float arriveRate) { //This is the only way that a node can be constructed.

       double currTime = 0;

       while (currTime < popTime) {
         currTime += expVar(arriveRate);
         frameQueue.push_back(currTime);
       }
    }

    double next() {
      if (frameQueue.empty()) {
        return T + 10;  //A value higher than the simulation time to indicate queue is empty numerically
      } else {
        return frameQueue.front();
      }
    }

    void send() { //De-queue the next frame successfully and therefore also reset the collision counter.
      frameQueue.pop_front();
      i = 0;
    }

    bool isEmpty() {return frameQueue.empty();}

    bool backOff(double propTime) {  //We need to input a "base time" to account for the propagation delay.
      ++i;

      if (i > 10) {
        frameQueue.pop_front();
        i = 0;
        return false;
      } else {
        k = 0;
        frameQueue[0] = (double)(rand() % (pow(2, i)-2) + 1)/R + propTime; //Returns random number in [1,(2^i)-1], inclusive
        ++k;
        while (frameQueue[k] <= frameQueue[0]) {
          frameQueue[k] = frameQueue[0];
        }
        return true;
      }

    }

    bool wait(double transTime) { //Makes the node wait until the channel is clear of another transmission

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

  double propTime = (double)D/S;  //Propagation time between two adjacent nodes, pre-computed.

  int numAttempts = 0;
  int numSuccesses = 0;
  int numDropped = 0;

  vector<node> LAN;

  for (int i = 0; i < N; i++) {  //Pre-simulation, populate the queues of all nodes until T seconds.
    LAN.push_back(node(T, A));
  }

  double simTime = 0;

  int sender = findMin(LAN); //The index of the node trying to send

  int maxProp = 0;  //Maximum distance (in tens of metres) from sender to a colliding node. This is used as the base backoff time for each node in the collision event.

  vector<int> collidedNodes;  //Indexes of nodes that will experience a collision.

  while ((LAN[sender].next()) < T) {

    maxProp = 0;

    for (int g = 0; g < LAN.size(); g++) {

      if (LAN[g].next() <= (LAN[sender].next() + (abs(sender - g) * propTime))) { //Case that the first bit will arrive at a node while that node is sending (collision)

        collidedNodes.push_back(g);

        if (abs(sender - g) > maxProp) {
          maxProp = abs(sender - g);
        }

      }

    }

    for (int g = 0; g < collidedNodes.size(); g++) {  //Initiate the backoff procedure for each node caught in the collision
      if (!(LAN[g].backOff((double)maxProp * propTime))) {  //This must be in a loop separate from the collision detection loop because we don't know the timestampt to start the backoff until we know all nodes involved in the collision!
        ++numDropped;
      }
    }

  //if maxProp == 0 after the collision detection loop, it must be the case that there is no collision at all.

  

    collidedNodes.clear();
    sender = findMin(LAN);  //Find the attempting sender for the next loop
  }


  return 0;
}
