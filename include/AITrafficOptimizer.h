#ifndef AI_TRAFFIC_OPTIMIZER_H
#define AI_TRAFFIC_OPTIMIZER_H

#include "TrafficStrategy.h"
#include <vector>

class AITrafficOptimizer : public TrafficStrategy {
private:
    float densityWeight;
    float waitTimeWeight;
    float priorityWeight;

    int currentActiveRoad;
    int targetRoad;
    int lastSwitchTime;
    bool inYellow;
    bool inAllRed;

public:
    AITrafficOptimizer();
    void update(Intersection* intersection, int currentTime) override;
    
    // AI "Thinking" process
    float calculateScore(Road* road);
};

#endif
