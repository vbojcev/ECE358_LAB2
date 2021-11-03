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
    return -1;
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

void node::collide() {
  collisions++;
}

//Back off for a random time
bool node::backOff() {
  if (collisions > 10) {
    send();
    return false;
  }

  double delay = ((double) (rand() % (int) pow(2, collisions))*512)/(double) R + frameQueue.front();
  wait(delay);
  return true;
}

//Pushes all packets back to delay time
void node::wait(double delay) { //Makes the node wait until the channel is clear of another transmission
  for (int i = 0; i < frameQueue.size(); i++) {
    frameQueue[i] = min(frameQueue[i], delay);
  }
}
