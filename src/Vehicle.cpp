#include "Vehicle.h"
#include <cmath>
using namespace std;


Vehicle::Vehicle(int id, int arrivalTime) 
    : id(id), arrivalTime(arrivalTime), waitingTime(0), posX(0), posY(0), speed(0), targetSpeed(2.0f), angle(0), state(APPROACHING) {}

Vehicle::~Vehicle() {}

void Vehicle::move() {
    float rad = angle * 3.14159f / 180.0f;
    posX += cos(rad) * speed;
    posY += sin(rad) * speed;
    
    // Smooth acceleration/deceleration
    if (speed < targetSpeed) speed += 0.1f;
    else if (speed > targetSpeed) speed -= 0.2f;
    if (speed < 0) speed = 0;
}

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
