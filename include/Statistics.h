#ifndef STATISTICS_H
#define STATISTICS_H

#include <vector>
using namespace std;


class Statistics {
private:
    int totalVehiclesPassed;
    long long totalWaitTime;
    int maxQueueLength;

public:
    Statistics();
    void recordVehiclePass(int waitTime);
    void updateMaxQueue(int currentQueue);
    
    int getTotalVehiclesPassed() const { return totalVehiclesPassed; }
    double getAverageWaitingTime() const;
    int getMaxQueueLength() const { return maxQueueLength; }

    void displayReport() const;
};

#endif
