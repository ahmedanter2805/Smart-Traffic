#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QPainter>
#include <vector>
#include <QPixmap>
#include "Intersection.h"
#include "TrafficController.h"
#include "Statistics.h"
#include "TrafficStrategy.h"
#include "AITrafficOptimizer.h"
#include "HardwareBridge.h"
#include "FireTruck.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateSimulation();
    void toggleStrategy();
    void toggleHardware();
    void spawnVehicle(QString type);

private:
    Ui::MainWindow *ui;
    
    // City Simulation: Single Complex Intersection
    std::vector<Intersection*> intersections;
    std::vector<TrafficController*> controllers;
    
    FixedTimeStrategy* fixedStrategy;
    AITrafficOptimizer* smartStrategy;
    Statistics stats;
    
    QTimer *timer;
    int currentStep;
    int vehicleIdCounter;
    int violationCount = 0;
    bool isSmartMode;
    
    HardwareBridge* hwBridge;
    bool hwEnabled = false;

    // Assets
    QPixmap pixNormal;
    QPixmap pixAmbulance;
    QPixmap pixPolice;
    QPixmap pixFireTruck;
};

#endif // MAINWINDOW_H
