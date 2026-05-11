#include "Road.h"
#include <cmath>
#include <algorithm>
using namespace std;


Road::Road(string name, float sx, float sy, float slx, float sly, float angle) 
    : name(name), startX(sx), startY(sy), stopLineX(slx), stopLineY(sly), angle(angle), laneWidth(60.0f) {}

Road::~Road() {
    for (Vehicle* v : vehicles) delete v;
    vehicles.clear();
}

void Road::addVehicle(Vehicle* v) {
    v->setAngle(angle);
    v->setState(APPROACHING);
    
    // Lane Assignment: 0=Inner, 1=Middle, 2=Emergency (Outer)
    if (v->isPriority()) v->setLane(2);
    else v->setLane(rand() % 2);

    float offset = (v->getLane() - 1) * 60.0f;
    // Calibration for each direction to keep Lane 0 INNER
    if (angle == 90) offset = offset;   // North-bound -> Inner is 730 (790-60)
    if (angle == 270) offset = -offset; // South-bound -> Inner is 670 (610+60)
    if (angle == 0) offset = -offset;   // West-bound -> Inner is 420 (360+60)
    if (angle == 180) offset = offset;  // East-bound -> Inner is 480 (540-60)

    float perpRad = (angle + 90.0f) * 3.14159f / 180.0f;
    v->setPosition(startX + cos(perpRad) * offset, startY + sin(perpRad) * offset);
    
    bool clear = true;
    for(auto other : vehicles) {
        float dx = v->getX() - other->getX();
        float dy = v->getY() - other->getY();
        if(sqrt(dx*dx + dy*dy) < 160) { clear = false; break; }
    }
    
    if(clear) vehicles.push_back(v);
    else delete v; 
}

Vehicle* Road::removeVehicle() {
    for (auto v : vehicles) {
        if (v->getState() == WAITING || v->getState() == APPROACHING) {
            v->setState(CROSSING);
            return v;
        }
    }
    return nullptr;
}

bool Road::hasPriorityVehicle() const {
    for (Vehicle* v : vehicles) {
        if (v->isPriority() && (v->getState() == WAITING || v->getState() == APPROACHING)) return true;
    }
    return false;
}

int Road::getVehicleCount() const {
    int count = 0;
    for (auto v : vehicles) if (v->getState() != EXITING) count++;
    return count;
}

string Road::getName() const {
    return name;
}

void Road::updateWaitingTimes() {
    for (Vehicle* v : vehicles) {
        if (v->getState() == WAITING) v->incrementWait();
    }
}

float Road::getLaneOffset(Vehicle* v) {
    return (v->getLane() - 1) * 60.0f;
}

void Road::updatePositions(bool isGreen, bool isYellow, bool isIntersectionBusy) {
    for (size_t i = 0; i < vehicles.size(); ++i) {
        Vehicle* v = vehicles[i];
        
        float offset = (v->getLane() - 1) * 60.0f;
        if (angle == 270 || angle == 0) offset = -offset;

        float perpRad = (angle + 90.0f) * 3.14159f / 180.0f;
        float curStopX = stopLineX + cos(perpRad) * offset;
        float curStopY = stopLineY + sin(perpRad) * offset;

        float distToLine = 0;
        if (angle == 90) distToLine = curStopY - (v->getY() + 25);
        else if (angle == 270) distToLine = (v->getY() - 25) - curStopY;
        else if (angle == 0) distToLine = curStopX - (v->getX() + 25);
        else if (angle == 180) distToLine = (v->getX() - 25) - curStopX;
        
        float targetS = 3.5f; 

        // 1. Logic State Control
        if (v->getState() == APPROACHING) {
            bool lightAllows = isGreen || (isYellow && distToLine < 30);
            bool safeToEnter = !isIntersectionBusy;

            if (distToLine < 60) { // Start stopping earlier
                if (lightAllows && safeToEnter && distToLine < 10) {
                    v->setState(CROSSING);
                } else if (!lightAllows || !safeToEnter) {
                    targetS = 0;
                    if (distToLine < 10) v->setState(WAITING);
                }
            }
        } 
        else if (v->getState() == WAITING) {
            if (isGreen && !isIntersectionBusy) {
                v->setState(CROSSING);
            } else {
                targetS = 0;
                v->setSpeed(0); // Kill momentum to prevent sliding into intersection
            }
        } 
        else if (v->getState() == CROSSING) {
            targetS = 4.0f;
        }

        // 2. Strict Collision Avoidance
        for (auto other : vehicles) {
            if (other == v) continue;
            if (other->getLane() != v->getLane()) continue;

            float adx = other->getX() - v->getX();
            float ady = other->getY() - v->getY();
            float rad = angle * 3.14159f / 180.0f;
            float proj = adx * cos(rad) + ady * sin(rad);

            if (proj > 0 && proj < 100) { 
                targetS = min(targetS, other->getSpeed() * 0.5f);
                if (proj < 75) {
                    targetS = 0;
                    if (proj < 55) v->setSpeed(0); // Hard stop to prevent overlap
                }
            }
        }

        v->setTargetSpeed(targetS);
        v->move();
        
        if (v->getState() == CROSSING && distToLine < -300) {
            v->setState(EXITING);
        }
    }

    // Cleanup off-screen
    auto it = std::remove_if(vehicles.begin(), vehicles.end(), [](Vehicle* v) {
        if (v->getX() < -300 || v->getX() > 1700 || v->getY() < -300 || v->getY() > 1200) {
            delete v;
            return true;
        }
        return false;
    });
    vehicles.erase(it, vehicles.end());
}
