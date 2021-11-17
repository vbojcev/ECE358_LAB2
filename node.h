#include <iostream>

using namespace std;

class node {
  private:
    int collisions;
    int npCounter;
    int T;
    int R;
    deque<double> frameQueue;

  public:
    node(float A, int T, int R);
    double next();
    void send();
    bool isEmpty();
    void collide();
    bool backOff(double baseTime);
    bool npBackOff(double baseTime);
    void wait(double delay);
    int getSize();
    void resetNP();
};