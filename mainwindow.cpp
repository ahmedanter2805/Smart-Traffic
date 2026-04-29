#include "mainwindow.h"
#include "ui_mainwindow.h"
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
    setWindowTitle("Smart Traffic Mega City Grid");
    resize(1400, 900);

    fixedStrategy = new FixedTimeStrategy(5);
    smartStrategy = new SmartAdaptiveStrategy();

    // Initialize 6 Intersections (3 Columns x 2 Rows)
    for (int i = 0; i < 6; ++i) {
        Intersection* inter = new Intersection();
        inter->addRoad(new Road("North"));
        inter->addRoad(new Road("South"));
        inter->addRoad(new Road("West"));
        inter->addRoad(new Road("East"));
        for(int l=0; l<4; ++l) inter->addLight(new TrafficLight());
        
        intersections.push_back(inter);
        
        TrafficController* ctrl = new TrafficController(inter);
        ctrl->setStrategy(fixedStrategy);
        controllers.push_back(ctrl);
    }

    // UI Control Overlay
    QWidget *overlay = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(overlay);
    QPushButton *btnToggle = new QPushButton("Switch to Smart Mode", this);
    btnToggle->setFixedSize(200, 40);
    btnToggle->setStyleSheet("background-color: #2c3e50; color: white; border-radius: 10px; font-weight: bold;");
    connect(btnToggle, &QPushButton::clicked, this, &MainWindow::toggleStrategy);
    layout->addWidget(btnToggle);
    overlay->setLayout(layout);
    overlay->setGeometry(10, 10, 220, 100);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateSimulation);
    timer->start(150); 
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
        btn->setText("Switch to Fixed Mode");
        btn->setStyleSheet("background-color: #e67e22; color: white; border-radius: 10px; font-weight: bold;");
    } else {
        btn->setText("Switch to Smart Mode");
        btn->setStyleSheet("background-color: #2c3e50; color: white; border-radius: 10px; font-weight: bold;");
    }
}

void MainWindow::updateSimulation() {
    currentStep++;

    // Random Arrivals (Increased to 22% for slightly more traffic)
    for (auto inter : intersections) {
        for (Road* road : inter->getRoads()) {
            if (rand() % 100 < 22) { 
                Vehicle* v;
                int r = rand() % 100;
                if (r < 15) v = new Ambulance(vehicleIdCounter++, currentStep); // 15% Ambulance
                else if (r < 30) v = new PoliceCar(vehicleIdCounter++, currentStep); // 15% Police
                else v = new NormalVehicle(vehicleIdCounter++, currentStep);
                road->addVehicle(v);
            }
        }
    }

    for (auto ctrl : controllers) ctrl->runStep(currentStep);

    for (auto inter : intersections) {
        auto& roads = inter->getRoads();
        auto& lights = inter->getLights();
        for (size_t i = 0; i < roads.size(); ++i) {
            if (lights[i]->getState() == LightState::GREEN && roads[i]->getVehicleCount() > 0) {
                Vehicle* v = roads[i]->removeVehicle();
                stats.recordVehiclePass(v->getWaitingTime());
                delete v;
            }
            roads[i]->updateWaitingTimes();
        }
    }
    update();
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 1. Landscape
    painter.fillRect(rect(), QColor("#2e7d32")); 

    int roadW = 80;
    int cols = 3;
    int rows = 2;
    int xSpacing = width() / (cols + 1);
    int ySpacing = height() / (rows + 1);

    // 2. Buildings
    painter.setPen(QPen(Qt::black, 1));
    for (int x = 60; x < width(); x += 180) {
        for (int y = 60; y < height(); y += 180) {
            bool onRoad = false;
            for(int i=1; i<=cols; ++i) if(abs(x - i*xSpacing) < 80) onRoad = true;
            for(int j=1; j<=rows; ++j) if(abs(y - j*ySpacing) < 80) onRoad = true;

            if (!onRoad) {
                painter.setBrush(QColor("#546e7a"));
                painter.drawRect(x, y, 60, 80);
                painter.setBrush(QColor("#fff59d"));
                for(int wx=x+8; wx<x+50; wx+=15) {
                    for(int wy=y+10; wy<y+70; wy+=20) painter.drawRect(wx, wy, 8, 10);
                }
                painter.setBrush(QColor("#37474f"));
                QPolygon roof; roof << QPoint(x-5, y) << QPoint(x+65, y) << QPoint(x+30, y-20);
                painter.drawPolygon(roof);
            }
        }
    }

    // 3. Road Grid
    painter.setPen(Qt::NoPen);
    for (int i = 1; i <= cols; ++i) {
        int x = i * xSpacing;
        painter.setBrush(QColor("#212121")); painter.drawRect(x - roadW/2 - 4, 0, roadW + 8, height());
        painter.setBrush(QColor("#37474f")); painter.drawRect(x - roadW/2, 0, roadW, height());
    }
    for (int j = 1; j <= rows; ++j) {
        int y = j * ySpacing;
        painter.setBrush(QColor("#212121")); painter.drawRect(0, y - roadW/2 - 4, width(), roadW + 8);
        painter.setBrush(QColor("#37474f")); painter.drawRect(0, y - roadW/2, width(), roadW);
    }
    painter.setPen(QPen(QColor("#fbc02d"), 3, Qt::DashLine));
    for (int i = 1; i <= cols; ++i) painter.drawLine(i * xSpacing, 0, i * xSpacing, height());
    for (int j = 1; j <= rows; ++j) painter.drawLine(0, j * ySpacing, width(), j * ySpacing);

    // 4. Detailed Assets
    struct P { int x, y, dx, dy, lx, ly; };
    for (int j = 0; j < rows; ++j) {
        for (int i = 0; i < cols; ++i) {
            int cX = (i + 1) * xSpacing;
            int cY = (j + 1) * ySpacing;
            Intersection* inter = intersections[j * cols + i];
            auto& roads = inter->getRoads();
            auto& lights = inter->getLights();
            P pos[4] = { {cX-20, cY-60, 0, -1, cX-55, cY-55}, {cX+20, cY+60, 0, 1, cX+55, cY+55}, {cX-60, cY+20, -1, 0, cX-55, cY-55}, {cX+60, cY-20, 1, 0, cX+55, cY+55} };

            for (int k = 0; k < 4; ++k) {
                painter.setBrush(QColor("#212121"));
                painter.drawRoundedRect(pos[k].lx-12, pos[k].ly-12, 24, 24, 3, 3);
                painter.setBrush(lights[k]->getState() == LightState::GREEN ? Qt::green : Qt::red);
                painter.drawEllipse(pos[k].lx-6, pos[k].ly-6, 12, 12);

                auto vqs = roads[k]->getVehicles();
                int off = 0;
                while (!vqs.empty()) {
                    Vehicle* v = vqs.front(); vqs.pop();
                    int vx, vy, lO = v->isPriority() ? 15 : -15;
                    if (pos[k].dy != 0) { vx = pos[k].x + lO; vy = pos[k].y + (pos[k].dy * off * 35); }
                    else { vx = pos[k].x + (pos[k].dx * off * 35); vy = pos[k].y + lO; }

                    // Car Body
                    painter.setBrush((v->getType()=="Ambulance")?Qt::white:(v->getType()=="Police"?QColor("#0d47a1"):QColor("#455a64")));
                    painter.setPen(QPen(Qt::black, 1));
                    painter.drawRoundedRect(vx-12, vy-12, 24, 24, 4, 4);

                    // Details: Windows
                    painter.setBrush(QColor(255, 255, 255, 180));
                    if (pos[k].dy != 0) painter.drawRect(vx-8, vy+(pos[k].dy==-1?-8:2), 16, 6);
                    else painter.drawRect(vx+(pos[k].dx==1?2:-8), vy-8, 6, 16);

                    // Details: Headlights & Tail lights
                    painter.setBrush(QColor("#ffee58")); // Front
                    if (pos[k].dy == -1) { painter.drawEllipse(vx-10, vy-13, 4, 4); painter.drawEllipse(vx+6, vy-13, 4, 4); }
                    else if (pos[k].dy == 1) { painter.drawEllipse(vx-10, vy+9, 4, 4); painter.drawEllipse(vx+6, vy+9, 4, 4); }
                    
                    painter.setBrush(Qt::red); // Back
                    if (pos[k].dy == -1) { painter.drawRect(vx-10, vy+9, 4, 2); painter.drawRect(vx+6, vy+9, 4, 2); }
                    else if (pos[k].dy == 1) { painter.drawRect(vx-10, vy-11, 4, 2); painter.drawRect(vx+6, vy-11, 4, 2); }

                    // Details: Side Mirrors
                    painter.setBrush(Qt::black);
                    if (pos[k].dy != 0) { painter.drawRect(vx-15, vy-3, 3, 6); painter.drawRect(vx+12, vy-3, 3, 6); }
                    else { painter.drawRect(vx-3, vy-15, 6, 3); painter.drawRect(vx-3, vy+12, 6, 3); }

                    if (v->isPriority()) {
                        bool flash = (currentStep/2)%2 == 0;
                        painter.setBrush(flash?Qt::red:Qt::blue);
                        painter.drawRect(vx-6, vy-15, 12, 5);
                    }
                    off++; if (off > 5) break;
                }
            }
        }
    }

    // 5. HUD
    painter.setPen(Qt::white);
    painter.setFont(QFont("Segoe UI", 12, QFont::Bold));
    painter.drawText(600, 30, "ULTIMATE CITY SIMULATOR | Step: " + QString::number(currentStep));
}
