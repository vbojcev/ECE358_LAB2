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
    int size();
    double next();
    void send();
    bool isEmpty();
    void collide();
    bool backOff();
    void wait(double delay);
};