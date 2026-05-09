#include "Road.h"
using namespace std;


Road::Road(string name) : name(name) {}

Road::~Road() {
    for (Vehicle* v : vehicleQueue) {
        delete v;
    }
    vehicleQueue.clear();
}

void Road::addVehicle(Vehicle* v) {
    vehicleQueue.push_back(v);
}

Vehicle* Road::removeVehicle() {
    if (vehicleQueue.empty()) return nullptr;
    
    // Bypass logic: Find the first priority vehicle and let it pass
    for (auto it = vehicleQueue.begin(); it != vehicleQueue.end(); ++it) {
        if ((*it)->isPriority()) {
            Vehicle* v = *it;
            vehicleQueue.erase(it);
            return v;
        }
    }
    
    // Default FIFO logic if no priority vehicle is found
    Vehicle* v = vehicleQueue.front();
    vehicleQueue.pop_front();
    return v;
}

bool Road::hasPriorityVehicle() const {
    for (Vehicle* v : vehicleQueue) {
        if (v->isPriority()) return true;
    }
    return false;
}

int Road::getVehicleCount() const {
    return vehicleQueue.size();
}

string Road::getName() const {
    return name;
}

void Road::updateWaitingTimes() {
    for (Vehicle* v : vehicleQueue) {
        v->incrementWait();
    }
}
