#ifndef ROAD_H
#define ROAD_H

#include <string>
#include <queue>
#include "Vehicle.h"

// Road Class (Demonstrates Encapsulation and Composition)
class Road {
private:
    std::string name;
    std::queue<Vehicle*> vehicleQueue;

public:
    Road(std::string name);
    ~Road();

    void addVehicle(Vehicle* v);
    Vehicle* removeVehicle();
    
    bool hasPriorityVehicle() const;
    int getVehicleCount() const;
    std::string getName() const;
    void updateWaitingTimes();
    
    // Added for visualization
    std::queue<Vehicle*> getVehicles() const { return vehicleQueue; }
};

#endif
