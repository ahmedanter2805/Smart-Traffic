#ifndef ROAD_H
#define ROAD_H

#include <string>
#include <vector>
#include "Vehicle.h"
using namespace std;


// Road Class (Demonstrates Encapsulation and Composition)
class Road {
private:
    string name;
    vector<Vehicle*> vehicles;
    float startX, startY;
    float stopLineX, stopLineY;
    float angle;

public:
    Road(string name, float sx, float sy, float slx, float sly, float angle);
    ~Road();

    void addVehicle(Vehicle* v);
    Vehicle* removeVehicle(); // Now just changes state to CROSSING
    
    bool hasPriorityVehicle() const;
    int getVehicleCount() const;
    string getName() const;
    void updateWaitingTimes();
    
    void updatePositions(bool isGreen, bool isYellow, bool isIntersectionBusy);
    vector<Vehicle*>& getAllVehicles() { return vehicles; }
    
    float getAngle() const { return angle; }
    float getStartX() const { return startX; }
    float getStartY() const { return startY; }
    float getLaneOffset(Vehicle* v);

private:
    float laneWidth;
};

#endif
