#ifndef FIRETRUCK_H
#define FIRETRUCK_H

#include "Vehicle.h"

class FireTruck : public PriorityVehicle {
public:
    FireTruck(int id, int startX, int startY, int lane) 
        : PriorityVehicle(id, startX, startY, lane) {
        speed = 4.5; // Fire trucks are fast!
    }

    string getType() const override { return "FireTruck"; }
};

#endif
