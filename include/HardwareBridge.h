#ifndef HARDWAREBRIDGE_H
#define HARDWAREBRIDGE_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>

class HardwareBridge : public QObject {
    Q_OBJECT
public:
    explicit HardwareBridge(QObject *parent = nullptr) : QObject(parent) {
        udpSocket = new QUdpSocket(this);
    }

    void setTargetIP(const QString& ip) {
        targetAddress = QHostAddress(ip);
    }

    void sendStatus(const QString& lightState, int carCount, int emergencyType = 0, int violatorId = -1) {
        if (targetAddress.isNull()) return;

        // Protocol: L:[State]|C:[Count]|E:[Type]|V:[ViolatorID]
        QString data = QString("L:%1|C:%2|E:%3|V:%4")
                        .arg(lightState)
                        .arg(carCount)
                        .arg(emergencyType)
                        .arg(violatorId);

        QByteArray datagram = data.toUtf8();
        udpSocket->writeDatagram(datagram, targetAddress, 4210);
    }

private:
    QUdpSocket *udpSocket;
    QHostAddress targetAddress;
};

#endif // HARDWAREBRIDGE_H
