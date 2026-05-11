#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>
using namespace std;


enum VehicleState { APPROACHING, WAITING, CROSSING, EXITING };

// Base Class: Vehicle (Demonstrates Abstraction and Encapsulation)
class Vehicle {
protected:
    int id;
    int arrivalTime;
    int waitingTime;
    float posX, posY;
    float speed;
    float targetSpeed;
    float angle;
    VehicleState state;
    int lane;

public:
    Vehicle(int id, int arrivalTime);
    virtual ~Vehicle();

    // Virtual function (Demonstrates Polymorphism)
    virtual bool isPriority() const = 0;
    virtual string getType() const = 0;

    void incrementWait();
    int getWaitingTime() const;
    int getId() const;

    // Movement methods
    void setPosition(float x, float y) { posX = x; posY = y; }
    float getX() const { return posX; }
    float getY() const { return posY; }
    void setAngle(float a) { angle = a; }
    float getAngle() const { return angle; }
    void setSpeed(float s) { speed = s; }
    float getSpeed() const { return speed; }
    void setTargetSpeed(float s) { targetSpeed = s; }
    void setState(VehicleState s) { state = s; }
    VehicleState getState() const { return state; }
    void setLane(int l) { lane = l; }
    int getLane() const { return lane; }
    
    virtual void move();
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
