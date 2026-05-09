#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>
using namespace std;


// Base Class: Vehicle (Demonstrates Abstraction and Encapsulation)
class Vehicle {
protected:
    int id;
    int arrivalTime;
    int waitingTime;

public:
    Vehicle(int id, int arrivalTime);
    virtual ~Vehicle();

    // Virtual function (Demonstrates Polymorphism)
    virtual bool isPriority() const = 0;
    virtual string getType() const = 0;

    void incrementWait();
    int getWaitingTime() const;
    int getId() const;
};

// Derived Class: NormalVehicle (Demonstrates Inheritance)
class NormalVehicle : public Vehicle {
public:
    NormalVehicle(int id, int arrivalTime);
    bool isPriority() const override;
    string getType() const override;
};

// Derived Class: PriorityVehicle (Demonstrates Inheritance)
class PriorityVehicle : public Vehicle {
public:
    PriorityVehicle(int id, int arrivalTime);
    bool isPriority() const override;
};

class Ambulance : public PriorityVehicle {
public:
    using PriorityVehicle::PriorityVehicle;
    string getType() const override { return "Ambulance"; }
};

class PoliceCar : public PriorityVehicle {
public:
    using PriorityVehicle::PriorityVehicle;
    string getType() const override { return "Police"; }
};

#endif
