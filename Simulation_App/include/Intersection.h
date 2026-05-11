#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <vector>
#include "Road.h"
#include "TrafficLight.h"
using namespace std;


class Intersection {
private:
    vector<Road*> roads;
    vector<TrafficLight*> lights;

public:
    Intersection();
    ~Intersection();

    void addRoad(Road* road);
    void addLight(TrafficLight* light);

    const vector<Road*>& getRoads() const;
    const vector<TrafficLight*>& getLights() const;

    void displayState() const;
};

#endif
