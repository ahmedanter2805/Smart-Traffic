#include "Statistics.h"
#include <iostream>

Statistics::Statistics() 
    : totalVehiclesPassed(0), totalWaitTime(0), maxQueueLength(0) {}

void Statistics::recordVehiclePass(int waitTime) {
    totalVehiclesPassed++;
    totalWaitTime += waitTime;
}

void Statistics::updateMaxQueue(int currentQueue) {
    if (currentQueue > maxQueueLength) {
        maxQueueLength = currentQueue;
    }
}

double Statistics::getAverageWaitingTime() const {
    if (totalVehiclesPassed == 0) return 0.0;
    return (double)totalWaitTime / totalVehiclesPassed;
}

void Statistics::displayReport() const {
    std::cout << "\n========== SIMULATION STATISTICS ==========" << std::endl;
    std::cout << "Total Vehicles Passed: " << totalVehiclesPassed << std::endl;
    if (totalVehiclesPassed > 0) {
        std::cout << "Average Waiting Time: " << (double)totalWaitTime / totalVehiclesPassed << " steps" << std::endl;
    } else {
        std::cout << "Average Waiting Time: 0 steps" << std::endl;
    }
    std::cout << "Maximum Queue Length Observed: " << maxQueueLength << std::endl;
    std::cout << "===========================================" << std::endl;
}
