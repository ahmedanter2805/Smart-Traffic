#include "Intersection.h"
#include <iostream>
#include <iomanip>
using namespace std;


Intersection::Intersection() {}

Intersection::~Intersection() {
    // Note: We don't delete roads and lights here as they might be managed elsewhere,
    // but in this simple project, the Controller or main will handle cleanup if needed.
}

void Intersection::addRoad(Road* road) {
    roads.push_back(road);
}

void Intersection::addLight(TrafficLight* light) {
    lights.push_back(light);
}

const vector<Road*>& Intersection::getRoads() const {
    return roads;
}

const vector<TrafficLight*>& Intersection::getLights() const {
    return lights;
}

void Intersection::displayState() const {
    cout << left << setw(15) << "Road Name" 
              << setw(10) << "Light" 
              << setw(10) << "Vehicles" << endl;
    cout << "------------------------------------------" << endl;
    
    for (size_t i = 0; i < roads.size(); ++i) {
        cout << left << setw(15) << roads[i]->getName() 
                  << setw(10) << lights[i]->getStateString() 
                  << setw(10) << roads[i]->getVehicleCount() << endl;
    }
    cout << "------------------------------------------" << endl;
}
