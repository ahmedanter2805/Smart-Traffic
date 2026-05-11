#ifndef TRAFFICCONTROLLER_H
#define TRAFFICCONTROLLER_H

#include "TrafficStrategy.h"
#include "Intersection.h"

class TrafficController {
private:
    TrafficStrategy* strategy;
    Intersection* intersection;

public:
    TrafficController(Intersection* intersection);
    void setStrategy(TrafficStrategy* newStrategy);
    void runStep(int currentTime);
    TrafficStrategy* getStrategy() const { return strategy; }
};

#endif
