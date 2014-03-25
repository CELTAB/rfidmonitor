#ifndef NETWORKCOMMUNICATION_H
#define NETWORKCOMMUNICATION_H

#include <QObject>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QByteArray>
#include <QAbstractSocket>

#include "settings.h"

class NetworkCommunication : public QObject
{
    Q_OBJECT

private:
    explicit NetworkCommunication(QObject *parent = 0);
    void processDatagram(QByteArray &datagram);
    void handshake(QByteArray byteArray);

public:
    static NetworkCommunication *instance();
    QUdpSocket *m_udpSocket;
    QTcpSocket *m_tcpSocket;
    int m_udpPort;
    int m_tcpPort;
    void startListeningBroadcast();
    void stopListeningBroadcast();
    void sendData(const QByteArray &data, const Settings::TcpDataType &type);
    void connectToRasp(QString ip, int port);
    void triggerToGetCurrentConfigFromRasp();
    void sendNewConfigToRasp(QByteArray json);

private slots:
    void udpDataAvailable();
    void tcpDataAvailable();
    void tcpSocketError(QAbstractSocket::SocketError socketError);
signals:
    void newRaspFound(QVariantMap raspInfo);
    void currentConfigFromRasp(QByteArray json);
    void newRFIDMontiorAnswer(QString answer);
    void connectionEstablished();
    void connectionFailed();
};

#endif // NETWORKCOMMUNICATION_H
