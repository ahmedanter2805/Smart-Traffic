#ifndef ROAD_H
#define ROAD_H

#include <string>
#include <queue>
#include <deque>
#include "Vehicle.h"
using namespace std;


// Road Class (Demonstrates Encapsulation and Composition)
class Road {
private:
    string name;
    deque<Vehicle*> vehicleQueue;

public:
    Road(string name);
    ~Road();

    void addVehicle(Vehicle* v);
    Vehicle* removeVehicle();
    
    bool hasPriorityVehicle() const;
    int getVehicleCount() const;
    string getName() const;
    void updateWaitingTimes();
    
    // Added for visualization
    queue<Vehicle*> getVehicles() const { return queue<Vehicle*>(vehicleQueue); }
};

#endif
