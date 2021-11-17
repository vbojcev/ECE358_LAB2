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

void node::npBackOff(int npCollisions) {
  int k = rand() % (int)pow(2, npCollisions);
  wait(next() + ((double) k*512)/(double)R);
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
