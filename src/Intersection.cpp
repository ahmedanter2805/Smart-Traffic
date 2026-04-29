#include "Intersection.h"
#include <iostream>
#include <iomanip>

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

const std::vector<Road*>& Intersection::getRoads() const {
    return roads;
}

const std::vector<TrafficLight*>& Intersection::getLights() const {
    return lights;
}

void Intersection::displayState() const {
    std::cout << "------------------------------------------" << std::endl;
    std::cout << std::left << std::setw(15) << "Road Name" 
              << std::setw(10) << "Light" 
              << std::setw(10) << "Vehicles" << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    
    for (size_t i = 0; i < roads.size(); ++i) {
        std::cout << std::left << std::setw(15) << roads[i]->getName() 
                  << std::setw(10) << lights[i]->getStateString() 
                  << std::setw(10) << roads[i]->getVehicleCount() << std::endl;
    }
    std::cout << "------------------------------------------" << std::endl;
}
