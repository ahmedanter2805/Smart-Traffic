#include "mainwindow.h"
#include "ui_mainwindow.h"
using namespace std;

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <ctime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentStep(0)
    , vehicleIdCounter(1)
    , isSmartMode(false)
{
    ui->setupUi(this);
    setWindowTitle("Smart Traffic AI Simulator");
    resize(1400, 900);

    // Load Assets from Resources
    pixNormal.load(":/assets/car_normal.png");
    pixAmbulance.load(":/assets/car_ambulance.png");
    pixPolice.load(":/assets/car_police.png");

    fixedStrategy = new FixedTimeStrategy(150, 60, 30); 
    smartStrategy = new AITrafficOptimizer();

    // Initialize Single Intersection (Wide 6-lane layout)
    Intersection* inter = new Intersection();
    // North (South-bound): Center X = 790, Stop at Y = 270
    inter->addRoad(new Road("North", 790, -100, 790, 270, 90));
    // South (North-bound): Center X = 610, Stop at Y = 630
    inter->addRoad(new Road("South", 610, 1000, 610, 630, 270));
    // West (East-bound): Center Y = 360, Stop at X = 520
    inter->addRoad(new Road("West", -100, 360, 520, 360, 0));
    // East (West-bound): Center Y = 540, Stop at X = 880
    inter->addRoad(new Road("East", 1500, 540, 880, 540, 180));

    for(int l=0; l<4; ++l) inter->addLight(new TrafficLight());
    intersections.push_back(inter);
    
    TrafficController* ctrl = new TrafficController(inter);
    ctrl->setStrategy(fixedStrategy);
    controllers.push_back(ctrl);

    hwEnabled = true;
    hwBridge = new HardwareBridge(this);
    hwBridge->setTargetIP("192.168.1.100"); // Default ESP32 IP

    // Modern Glassmorphism UI Control
    QWidget *overlay = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(overlay);
    
    QPushButton *btnToggle = new QPushButton("AI Mode: OFF", this);
    btnToggle->setFixedSize(220, 45);
    
    QPushButton *btnHardware = new QPushButton("Hardware Sync: ON", this);
    btnHardware->setFixedSize(220, 45);

    QString btnStyle = 
        "QPushButton {"
        "  background: rgba(44, 62, 80, 0.9);"
        "  color: #ecf0f1;"
        "  border: 1px solid rgba(255,255,255,0.1);"
        "  border-radius: 12px;"
        "  font-family: 'Segoe UI';"
        "  font-size: 14px;"
        "  font-weight: 600;"
        "}";

    btnToggle->setStyleSheet(btnStyle);
    btnHardware->setStyleSheet(btnStyle);
    btnHardware->setStyleSheet("QPushButton { background: rgba(52, 152, 219, 0.9); color: white; border-radius: 12px; }");

    connect(btnToggle, &QPushButton::clicked, this, &MainWindow::toggleStrategy);
    connect(btnHardware, &QPushButton::clicked, this, &MainWindow::toggleHardware);

    layout->addWidget(btnToggle);
    layout->addWidget(btnHardware);
    overlay->setLayout(layout);
    overlay->setGeometry(20, 20, 250, 150);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateSimulation);
    timer->start(33); 
}

MainWindow::~MainWindow() {
    delete ui;
    for(auto c : controllers) delete c;
    for(auto i : intersections) delete i;
    delete fixedStrategy;
    delete smartStrategy;
}

void MainWindow::toggleStrategy() {
    isSmartMode = !isSmartMode;
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    for(auto c : controllers) c->setStrategy(isSmartMode ? static_cast<TrafficStrategy*>(smartStrategy) : static_cast<TrafficStrategy*>(fixedStrategy));
    
    if (isSmartMode) {
        btn->setText("AI Mode: ON");
        btn->setStyleSheet(
            "QPushButton {"
            "  background: rgba(39, 174, 96, 0.9);"
            "  color: white;"
            "  border-radius: 12px;"
            "  font-weight: bold;"
            "  border: 1px solid rgba(255,255,255,0.3);"
            "}"
        );
    } else {
        btn->setText("AI Mode: OFF");
        btn->setStyleSheet(
            "QPushButton {"
            "  background: rgba(44, 62, 80, 0.9);"
            "  color: #ecf0f1;"
            "  border-radius: 12px;"
            "  font-weight: bold;"
            "  border: 1px solid rgba(255,255,255,0.1);"
            "}"
        );
    }
}

void MainWindow::toggleHardware() {
    hwEnabled = !hwEnabled;
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (hwEnabled) {
        btn->setText("Hardware Sync: ON");
        btn->setStyleSheet("QPushButton { background: rgba(52, 152, 219, 0.9); color: white; border-radius: 12px; }");
    } else {
        btn->setText("Hardware Sync: OFF");
        btn->setStyleSheet("QPushButton { background: rgba(44, 62, 80, 0.9); color: #ecf0f1; border-radius: 12px; }");
    }
}

void MainWindow::updateSimulation() {
    currentStep++;

    // Random Arrivals
    for (auto inter : intersections) {
        for (Road* road : inter->getRoads()) {
            if (rand() % 1000 < 15) { 
                Vehicle* v;
                int r = rand() % 100;
                if (r < 8) v = new Ambulance(vehicleIdCounter++, currentStep); 
                else if (r < 16) v = new PoliceCar(vehicleIdCounter++, currentStep); 
                else v = new NormalVehicle(vehicleIdCounter++, currentStep);
                road->addVehicle(v);
            }
        }
    }

    for (auto ctrl : controllers) ctrl->runStep(currentStep);

    for (auto inter : intersections) {
        auto& roads = inter->getRoads();
        auto& lights = inter->getLights();
        
        bool isBusy = false;
        int totalCars = 0;
        int emergencyType = 0; // 0:None, 1:Amb, 2:Pol

        for (Road* r : roads) {
            totalCars += r->getVehicleCount();
            for (Vehicle* v : r->getAllVehicles()) {
                if (v->getState() == CROSSING) { 
                    isBusy = true; 
                    if (v->getType() == "Ambulance") emergencyType = 1;
                    else if (v->getType() == "Police") emergencyType = 2;
                }
            }
        }

        for (size_t i = 0; i < roads.size(); ++i) {
            bool isGreen = (lights[i]->getState() == LightState::GREEN);
            bool isYellow = (lights[i]->getState() == LightState::YELLOW);
            roads[i]->updatePositions(isGreen, isYellow, isBusy);
        }

        // IoT Sync
        if (hwEnabled && currentStep % 5 == 0) {
            QString lState = "R";
            if (lights[0]->getState() == LightState::GREEN) lState = "G";
            else if (lights[0]->getState() == LightState::YELLOW) lState = "Y";
            hwBridge->sendStatus(lState, totalCars, emergencyType);
        }
    }
    update();
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // 1. Ground Gradient
    QLinearGradient grassGrad(0, 0, width(), height());
    grassGrad.setColorAt(0, QColor("#1b5e20"));
    grassGrad.setColorAt(1, QColor("#2e7d32"));
    painter.fillRect(rect(), grassGrad);

    int roadW = 360; 
    int centerX = 700;
    int centerY = 450;

    // 2. High-Quality Asphalt
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor("#263238"));
    painter.drawRect(0, centerY - 180, width(), 360); // Horizontal
    painter.drawRect(centerX - 180, 0, 360, height()); // Vertical

    // Road Markings (White dashed for lanes)
    painter.setPen(QPen(Qt::white, 2, Qt::DashLine));
    // Horizontal Lanes
    painter.drawLine(0, centerY - 120, width(), centerY - 120);
    painter.drawLine(0, centerY - 60, width(), centerY - 60);
    painter.drawLine(0, centerY + 60, width(), centerY + 60);
    painter.drawLine(0, centerY + 120, width(), centerY + 120);
    // Vertical Lanes
    painter.drawLine(centerX - 120, 0, centerX - 120, height());
    painter.drawLine(centerX - 60, 0, centerX - 60, height());
    painter.drawLine(centerX + 60, 0, centerX + 60, height());
    painter.drawLine(centerX + 120, 0, centerX + 120, height());

    // Yellow Center Lines (Solid)
    painter.setPen(QPen(QColor("#fbc02d"), 4));
    painter.drawLine(0, centerY, width(), centerY);
    painter.drawLine(centerX, 0, centerX, height());

    // Emergency Lane Highlights (Outermost lane)
    painter.setBrush(QColor(255, 0, 0, 40));
    painter.drawRect(0, centerY - 180, width(), 60); // West-bound Outer
    painter.drawRect(0, centerY + 120, width(), 60); // East-bound Outer
    painter.drawRect(centerX - 180, 0, 60, height()); // South-bound Outer? No.
    // Let's be precise:
    // North Road (moves South): Lanes 700-880. Outer is 820-880.
    painter.drawRect(centerX + 120, 0, 60, height()); 
    // South Road (moves North): Lanes 520-700. Outer is 520-580.
    painter.drawRect(centerX - 180, 0, 60, height());

    // Intersection Box
    painter.setBrush(QColor("#1a237e")); // Darker blue center
    painter.drawRect(centerX - 180, centerY - 180, 360, 360);
    painter.setPen(QPen(Qt::white, 4));
    painter.drawRect(centerX - 180, centerY - 180, 360, 360);

    // 3. Vehicles & Lights
    for (auto inter : intersections) {
        auto& roads = inter->getRoads();
        auto& lights = inter->getLights();
        
        QPoint lPos[4] = { 
            {centerX + 190, centerY - 220}, // North
            {centerX - 220, centerY + 190}, // South
            {centerX - 220, centerY - 220}, // West
            {centerX + 190, centerY + 190}  // East
        };

        for (size_t i = 0; i < 4; ++i) {
            painter.setBrush(QColor("#121212"));
            painter.drawRoundedRect(lPos[i].x(), lPos[i].y(), 30, 30, 8, 8);
            
            QColor lightColor;
            if (lights[i]->getState() == LightState::GREEN) lightColor = QColor("#00e676");
            else if (lights[i]->getState() == LightState::YELLOW) lightColor = QColor("#ffeb3b");
            else lightColor = QColor("#ff1744");
            
            painter.setBrush(lightColor);
            painter.drawEllipse(lPos[i].x() + 5, lPos[i].y() + 5, 20, 20);
        }

        for (Road* road : roads) {
            for (Vehicle* v : road->getAllVehicles()) {
                painter.save();
                painter.translate(v->getX(), v->getY());
                painter.rotate(v->getAngle());
                
                QColor bodyColor = QColor("#455a64");
                if (v->getType() == "Ambulance") bodyColor = Qt::white;
                else if (v->getType() == "Police") bodyColor = QColor("#0d47a1");

                painter.setOpacity(0.3);
                painter.setBrush(Qt::black);
                painter.drawRoundedRect(-28, -18, 56, 36, 12, 12);
                painter.setOpacity(1.0);

                painter.setBrush(bodyColor);
                painter.setPen(QPen(Qt::black, 2));
                painter.drawRoundedRect(-25, -15, 50, 30, 10, 10);

                painter.setBrush(QColor("#263238"));
                painter.drawRect(-15, -12, 25, 24); 
                
                if (v->isPriority()) {
                    bool flash = (currentStep / 4) % 2 == 0;
                    painter.setBrush(flash ? Qt::red : Qt::blue);
                    painter.drawEllipse(-5, -12, 10, 10);
                }
                painter.restore();
            }
        }
    }

    // 4. Glass HUD
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 180));
    painter.drawRoundedRect(20, height() - 100, 620, 80, 20, 20);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Segoe UI", 12, QFont::Bold));
    painter.drawText(40, height() - 65, "SMART CITY - TRAFFIC AI OPTIMIZER");
    painter.setFont(QFont("Segoe UI", 10));
    
    QString statusMsg = "Mode: " + QString(isSmartMode ? "AI Neural" : "Fixed Rotation");
    TrafficStrategy* currentStrategy = isSmartMode ? static_cast<TrafficStrategy*>(smartStrategy) : static_cast<TrafficStrategy*>(fixedStrategy);
    statusMsg += " | IoT: " + QString(hwEnabled ? "LINKED" : "OFF");
    statusMsg += " | " + QString::fromStdString(currentStrategy->getLastReason());
    
    painter.drawText(40, height() - 40, statusMsg);
}
