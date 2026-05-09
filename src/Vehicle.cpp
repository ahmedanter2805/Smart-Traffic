#include "Vehicle.h"
using namespace std;


Vehicle::Vehicle(int id, int arrivalTime) : id(id), arrivalTime(arrivalTime), waitingTime(0) {}

Vehicle::~Vehicle() {}

void Vehicle::incrementWait() {
    waitingTime++;
}

int Vehicle::getWaitingTime() const {
    return waitingTime;
}

int Vehicle::getId() const {
    return id;
}

// NormalVehicle implementation
NormalVehicle::NormalVehicle(int id, int arrivalTime) : Vehicle(id, arrivalTime) {}

bool NormalVehicle::isPriority() const {
    return false;
}

string NormalVehicle::getType() const {
    return "Normal";
}

// PriorityVehicle implementation
PriorityVehicle::PriorityVehicle(int id, int arrivalTime) : Vehicle(id, arrivalTime) {}

bool PriorityVehicle::isPriority() const {
    return true;
}
