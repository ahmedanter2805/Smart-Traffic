#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <vector>
#include "Road.h"
#include "TrafficLight.h"

class Intersection {
private:
    std::vector<Road*> roads;
    std::vector<TrafficLight*> lights;

public:
    Intersection();
    ~Intersection();

    void addRoad(Road* road);
    void addLight(TrafficLight* light);

    const std::vector<Road*>& getRoads() const;
    const std::vector<TrafficLight*>& getLights() const;

    void displayState() const;
};

#endif
