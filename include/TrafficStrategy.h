#ifndef TRAFFICSTRATEGY_H
#define TRAFFICSTRATEGY_H

#include "Intersection.h"

// Strategy Interface (Demonstrates Polymorphism)
class TrafficStrategy {
protected:
    string lastReason;
public:
    virtual ~TrafficStrategy() {}
    virtual void update(Intersection* intersection, int currentTime) = 0;
    string getLastReason() const { return lastReason; }
};

// Concrete Strategy: FixedTime (Static timing)
class FixedTimeStrategy : public TrafficStrategy {
private:
    int greenDuration;
    int yellowDuration;
    int allRedDuration;
    int currentActiveRoad;
    int lastSwitchTime;
    bool inYellow;
    bool inAllRed;

public:
    FixedTimeStrategy(int greenDuration, int yellowDuration, int allRedDuration);
    void update(Intersection* intersection, int currentTime) override;
};

// Concrete Strategy: SmartAdaptive (Dynamic timing)
class SmartAdaptiveStrategy : public TrafficStrategy {
public:
    void update(Intersection* intersection, int currentTime) override;
};

#endif
