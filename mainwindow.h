#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QPainter>
#include <vector>
#include "Intersection.h"
#include "TrafficController.h"
#include "Statistics.h"
#include "TrafficStrategy.h"

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

private:
    Ui::MainWindow *ui;
    
    // Mega City: 6 Intersections (3x2 Grid)
    std::vector<Intersection*> intersections;
    std::vector<TrafficController*> controllers;
    
    FixedTimeStrategy* fixedStrategy;
    SmartAdaptiveStrategy* smartStrategy;
    Statistics stats;
    
    QTimer *timer;
    int currentStep;
    int vehicleIdCounter;
    bool isSmartMode;
};

#endif // MAINWINDOW_H
