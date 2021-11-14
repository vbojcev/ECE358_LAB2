#include <iostream>

using namespace std;

class node {
  private:
    int collisions;
    int T;
    int R;
    deque<double> frameQueue;

  public:
    node(float A, int T, int R);
    double next();
    void send();
    bool isEmpty();
    bool backOff(double baseTime);
    void wait(double delay);
    int getSize();
};