#ifndef FIRETRUCK_H
#define FIRETRUCK_H

#include "Vehicle.h"

class FireTruck : public PriorityVehicle {
public:
    FireTruck(int id, int arrivalTime) 
        : PriorityVehicle(id, arrivalTime) {
        speed = 4.5;
    }

    string getType() const override { return "FireTruck"; }
};

#endif
