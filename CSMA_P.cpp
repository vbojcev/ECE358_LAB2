#include <iostream>  //For debugging
#include <cmath>  //For logarithm
#include <vector>  //Used only as variable-size array (not explicit queue)
#include <algorithm>  //Used to sort the events in order of occurence
#include <time.h>  //Used to set the seed of the random number generator
#include <fstream>  //Used to output to a CSV file
#include <climits>

using namespace std;

float expVar(float rate) {
  return (float)-(1/rate)*log(1-((float)(rand()%1000)/1000));
}

class node {

  public:

    vector<float> queue;  //The frame queue only needs to be a queue of timestamps

    
    node(int popTime, int arriveRate) {
       float currTime = 0;
       
       while (currTime < popTime) {
         currTime += expVar(arriveRate);
         queue.push_back(currTime);
       }
    }

};

int main(int argc, char* argv[]) {

  int initTime = (unsigned)time(nullptr);

  srand(initTime);  //Initialize seed for pseudorandom uniform generator

  int N = stoi(argv[1]);  //Number of nodes
  float A = stof(argv[2]);  //Average arrival rate for each node's buffer
  int T = stoi(argv[3]);
  int R = 1000000;  //Speed, in bit/s
  int L = 1500;  //Packet lengths, in bits
  int D = 10;  //Distance between nodes on channel, in metres
  int S = 200000000;  //Propagation speed, in m/s
  
  int numAttempts = 0;
  int numSuccesses = 0;
  
  vector<node> LAN;
  
  for (int i = 0; i < N; i++) {  //Pre-simulation, populate the queues of all nodes until T seconds.
    LAN.push_back(node(T, A));
  }
  

}