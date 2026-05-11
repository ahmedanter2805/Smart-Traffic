#include "TrafficController.h"

TrafficController::TrafficController(Intersection* intersection) 
    : strategy(nullptr), intersection(intersection) {}

void TrafficController::setStrategy(TrafficStrategy* newStrategy) {
    strategy = newStrategy;
}

void TrafficController::runStep(int currentTime) {
    if (strategy && intersection) {
        strategy->update(intersection, currentTime);
    }
}
