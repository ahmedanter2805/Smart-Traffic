#include "AITrafficOptimizer.h"
#include <iostream>
#include <algorithm>
using namespace std;

AITrafficOptimizer::AITrafficOptimizer() 
    : densityWeight(0.5f), waitTimeWeight(0.4f), priorityWeight(1.0f),
      currentActiveRoad(0), targetRoad(0), lastSwitchTime(0), inYellow(false), inAllRed(false) {}

float AITrafficOptimizer::calculateScore(Road* road) {
    float score = 0;
    // 1. Density & Wait Pressure
    for (auto v : road->getAllVehicles()) {
        if (v->getState() == WAITING) score += 5.0f + (v->getWaitingTime() * 0.1f);
        else score += 2.0f;
    }
    // 2. High Priority Impact
    if (road->hasPriorityVehicle()) score += 100.0f;

    return score;
}

void AITrafficOptimizer::update(Intersection* intersection, int currentTime) {
    auto& roads = intersection->getRoads();
    auto& lights = intersection->getLights();
    int elapsed = currentTime - lastSwitchTime;

    if (!inYellow && !inAllRed) {
        float pressureNS = calculateScore(roads[0]) + calculateScore(roads[1]);
        float pressureWE = calculateScore(roads[2]) + calculateScore(roads[3]);

        // Hysteresis: Favor current axis to avoid frequent switching
        if (currentActiveRoad == 0) pressureNS += 25.0f;
        else pressureWE += 25.0f;

        int bestAxis = (pressureNS >= pressureWE) ? 0 : 1;

        // Switch only if significant pressure difference and min-green met
        if (bestAxis != currentActiveRoad && elapsed > 300) { 
            inYellow = true;
            targetRoad = bestAxis;
            lastSwitchTime = currentTime;
        }
        lastReason = "Neural Matrix: Analyzing Axis Flow... (NS:" + to_string((int)pressureNS) + " vs WE:" + to_string((int)pressureWE) + ")";
    } else if (inYellow) {
        if (elapsed >= 90) { // ~3s Yellow
            inYellow = false;
            inAllRed = true;
            lastSwitchTime = currentTime;
        }
        lastReason = "Neural Switch: CAUTION - Clearing Axis " + string(currentActiveRoad == 0 ? "N-S" : "W-E");
    } else if (inAllRed) {
        if (elapsed >= 45) { // ~1.5s All-Red
            inAllRed = false;
            currentActiveRoad = targetRoad;
            lastSwitchTime = currentTime;
        }
        lastReason = "Neural Switch: ALL-RED - Intersection Locked";
    }

    // Apply strict safety states
    for (size_t i = 0; i < lights.size(); ++i) {
        bool isCurrentAxis = (currentActiveRoad == 0) ? (i < 2) : (i >= 2);
        if (inAllRed) lights[i]->setState(LightState::RED);
        else if (inYellow && isCurrentAxis) lights[i]->setState(LightState::YELLOW);
        else lights[i]->setState(isCurrentAxis ? LightState::GREEN : LightState::RED);
    }
}
