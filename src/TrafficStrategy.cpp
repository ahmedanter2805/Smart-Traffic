#include "TrafficStrategy.h"
#include <iostream>

// FixedTimeStrategy implementation
FixedTimeStrategy::FixedTimeStrategy(int green, int yellow, int allRed) 
    : greenDuration(green), yellowDuration(yellow), allRedDuration(allRed), 
      currentActiveRoad(0), lastSwitchTime(0), inYellow(false), inAllRed(false) {}

void FixedTimeStrategy::update(Intersection* intersection, int currentTime) {
    auto& lights = intersection->getLights();
    int elapsed = currentTime - lastSwitchTime;

    if (!inYellow && !inAllRed) {
        if (elapsed >= greenDuration) { inYellow = true; lastSwitchTime = currentTime; }
    } else if (inYellow) {
        if (elapsed >= yellowDuration) { inYellow = false; inAllRed = true; lastSwitchTime = currentTime; }
    } else if (inAllRed) {
        if (elapsed >= allRedDuration) {
            inAllRed = false;
            currentActiveRoad = (currentActiveRoad == 0) ? 1 : 0; // Toggle between Axis 0 and 1
            lastSwitchTime = currentTime;
        }
    }

    // Phase 0: North & South (Lights 0,1) | Phase 1: West & East (Lights 2,3)
    for (size_t i = 0; i < lights.size(); ++i) {
        bool isThisAxis = (currentActiveRoad == 0) ? (i < 2) : (i >= 2);
        
        if (inAllRed) {
            lights[i]->setState(LightState::RED);
            lastReason = "CLEARANCE: All Red";
        } else if (inYellow) {
            if (isThisAxis) {
                lights[i]->setState(LightState::YELLOW);
                lastReason = "CAUTION: Switching Axis...";
            } else {
                lights[i]->setState(LightState::RED);
            }
        } else {
            lights[i]->setState(isThisAxis ? LightState::GREEN : LightState::RED);
            lastReason = "PASS: " + string(currentActiveRoad == 0 ? "North-South Axis" : "East-West Axis");
        }
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
