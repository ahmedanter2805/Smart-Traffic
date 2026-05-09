#include "Statistics.h"
#include <iostream>
using namespace std;


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
    cout << "\n========== SIMULATION STATISTICS ==========" << endl;
    cout << "Total Vehicles Passed: " << totalVehiclesPassed << endl;
    if (totalVehiclesPassed > 0) {
        cout << "Average Waiting Time: " << (double)totalWaitTime / totalVehiclesPassed << " steps" << endl;
    } else {
        cout << "Average Waiting Time: 0 steps" << endl;
    }
    cout << "Maximum Queue Length Observed: " << maxQueueLength << endl;
    cout << "===========================================" << endl;
}
