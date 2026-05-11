#ifndef HARDWAREBRIDGE_H
#define HARDWAREBRIDGE_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>

class HardwareBridge : public QObject {
    Q_OBJECT
public:
    explicit HardwareBridge(const QString& ip, QObject *parent = nullptr) : QObject(parent) {
        udpSocket = new QUdpSocket(this);
        setTargetIP(ip);
    }

    void setTargetIP(const QString& ip) {
        targetAddress = QHostAddress(ip);
    }

    void sendStatus(const QString& lightStates, int carCount, int emergencyType = 0, int violatorId = -1) {
        if (targetAddress.isNull()) return;

        // Protocol: L:[R1R2R3R4]|C:[Count]|E:[Type]|V:[ViolatorID]
        QString data = QString("L:%1|C:%2|E:%3|V:%4")
                        .arg(lightStates)
                        .arg(carCount)
                        .arg(emergencyType)
                        .arg(violatorId);

        udpSocket->writeDatagram(data.toUtf8(), targetAddress, 4210);
    }

private:
    QUdpSocket *udpSocket;
    QHostAddress targetAddress;
};

#endif
