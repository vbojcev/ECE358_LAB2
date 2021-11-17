#include <iostream>
#include <cmath>
#include <queue>
#include "node.h"

using namespace std;

//Generates exponentially distributed value
double expVar(float rate) {
  return (double)-(1/rate)*log(1-((double)(rand()%1000)/1000));
}

//Constructor
node::node(float A, int T, int R) {
  collisions = 0;
  this->T = T;
  this->R = R;
  double currTime = 0;

  //Append all arrivals in simulation to node
  while (currTime < T) {
    currTime += expVar(A);
    frameQueue.push_back(currTime);
  }
}

//Returns timestamp of next packet
double node::next() {
  if (frameQueue.empty()) {
    return T+1000;
  }
  return frameQueue.front();
}

//Removes next packet and reset collision counter
void node::send() {
  frameQueue.pop_front();
  collisions = 0;
  npCounter = 0;
}

bool node::isEmpty() {
  return frameQueue.empty();
}

//Back off for a random time
bool node::backOff(double baseTime) {

  collisions++;

  int k = rand() % (int)pow(2, collisions);

  if (collisions > 10) {
    send();
    wait(baseTime);
    return false;
  }
  wait(baseTime + ((double) k*512)/(double)R);
  return true;
}

bool node::npBackOff(double baseTime) {

  if (frameQueue[0] < baseTime) { //Case that packet is queued during transmission
    ++npCounter;

    int k = rand() % (int)pow(2, npCounter);

    if (npCounter > 10) {
      send();
      wait(baseTime);
      return false;
    } else {

      while (frameQueue[0] <= baseTime) {
        int k = rand() % (int)pow(2, npCounter);
        wait(frameQueue[0] + ((double) k*512)/(double)R);
        npCounter++;
        //This loop covers the case that it may take more than one backoff cycle before the channel is not busy.
        //That case is possible if k is 0, 1, or 2 (since k=3 implies 1536 bit-times which is longer than a packet of 1500 bit-times)
        if (npCounter > 10) {
          send();
          return false;
        }
      }
      return true;
    }
  } else {
    return true;
  }

}

//Pushes all packets back to delay time
void node::wait(double delay) { //Makes the node wait until the channel is clear of another transmission, also used for propagation delay.
  int i = 0;
  while (i < frameQueue.size() && frameQueue[i] < delay) {
    frameQueue[i] = delay;
    i++;
  }
}

int node::getSize() {
  return frameQueue.size();
}

void node::resetNP() {
  npCounter = 0;
}
