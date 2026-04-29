#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <string>

enum class LightState { RED, YELLOW, GREEN };

class TrafficLight {
private:
    LightState state;

public:
    TrafficLight();
    void setState(LightState newState);
    LightState getState() const;
    std::string getStateString() const;
};

#endif
