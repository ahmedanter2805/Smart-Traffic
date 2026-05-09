#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <string>
using namespace std;


enum class LightState { RED, YELLOW, GREEN };

class TrafficLight {
private:
    LightState state;

public:
    TrafficLight();
    void setState(LightState newState);
    LightState getState() const;
    string getStateString() const;
};

#endif
