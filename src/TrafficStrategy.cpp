#include "TrafficStrategy.h"
#include <iostream>

// FixedTimeStrategy implementation
FixedTimeStrategy::FixedTimeStrategy(int duration) 
    : greenDuration(duration), currentActiveRoad(0), lastSwitchTime(0) {}

void FixedTimeStrategy::update(Intersection* intersection, int currentTime) {
    auto& lights = intersection->getLights();
    
    // Switch road if duration passed
    if (currentTime - lastSwitchTime >= greenDuration) {
        currentActiveRoad = (currentActiveRoad + 1) % lights.size();
        lastSwitchTime = currentTime;
    }
    lastReason = "Fixed Rotation (Next in " + to_string(greenDuration - (currentTime - lastSwitchTime)) + "s)";

    // Set lights
    for (size_t i = 0; i < lights.size(); ++i) {
        lights[i]->setState(i == (size_t)currentActiveRoad ? LightState::GREEN : LightState::RED);
    }
}

void SmartAdaptiveStrategy::update(Intersection* intersection, int currentTime) {
    auto& roads = intersection->getRoads();
    auto& lights = intersection->getLights();

    int bestRoad = -1;
    bool priorityFound = false;

    for (size_t i = 0; i < roads.size(); ++i) {
        if (roads[i]->hasPriorityVehicle()) {
            bestRoad = i;
            priorityFound = true;
            lastReason = "EMERGENCY: " + roads[i]->getName();
            break; 
        }
    }

    if (!priorityFound) {
        int maxVehicles = -1;
        for (size_t i = 0; i < roads.size(); ++i) {
            if (roads[i]->getVehicleCount() > maxVehicles) {
                maxVehicles = roads[i]->getVehicleCount();
                bestRoad = i;
            }
        }
        if (maxVehicles > 0)
            lastReason = "DENSITY: " + roads[bestRoad]->getName();
        else
            lastReason = "IDLE (No Traffic)";
    }

    for (size_t i = 0; i < lights.size(); ++i) {
        lights[i]->setState(i == (size_t)bestRoad && roads[i]->getVehicleCount() > 0 ? LightState::GREEN : LightState::RED);
    }
}
