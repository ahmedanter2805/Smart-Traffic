#include "Road.h"

Road::Road(std::string name) : name(name) {}

Road::~Road() {
    while (!vehicleQueue.empty()) {
        delete vehicleQueue.front();
        vehicleQueue.pop();
    }
}

void Road::addVehicle(Vehicle* v) {
    vehicleQueue.push(v);
}

Vehicle* Road::removeVehicle() {
    if (vehicleQueue.empty()) return nullptr;
    Vehicle* v = vehicleQueue.front();
    vehicleQueue.pop();
    return v;
}

bool Road::hasPriorityVehicle() const {
    // Check if any vehicle in the queue is a priority vehicle
    // Note: In a real simulation, we might only check the first few vehicles
    // but for simplicity, we check the whole queue.
    std::queue<Vehicle*> temp = vehicleQueue;
    while (!temp.empty()) {
        if (temp.front()->isPriority()) return true;
        temp.pop();
    }
    return false;
}

int Road::getVehicleCount() const {
    return vehicleQueue.size();
}

std::string Road::getName() const {
    return name;
}

void Road::updateWaitingTimes() {
    std::queue<Vehicle*> temp;
    while (!vehicleQueue.empty()) {
        Vehicle* v = vehicleQueue.front();
        v->incrementWait();
        temp.push(v);
        vehicleQueue.pop();
    }
    vehicleQueue = temp;
}
