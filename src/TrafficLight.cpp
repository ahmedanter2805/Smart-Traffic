#include "TrafficLight.h"

TrafficLight::TrafficLight() : state(LightState::RED) {}

void TrafficLight::setState(LightState newState) {
    state = newState;
}

LightState TrafficLight::getState() const {
    return state;
}

std::string TrafficLight::getStateString() const {
    switch (state) {
        case LightState::RED: return "RED";
        case LightState::YELLOW: return "YELLOW";
        case LightState::GREEN: return "GREEN";
        default: return "UNKNOWN";
    }
}
