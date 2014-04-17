#ifndef NETWORKCOMMUNICATION_H
#define NETWORKCOMMUNICATION_H

#include <QObject>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QByteArray>
#include <QAbstractSocket>
#include <QTimer>
#include <QMap>
#include <QJsonObject>

#include "settings.h"

class NetworkCommunication : public QObject
{
    Q_OBJECT

private:
    explicit NetworkCommunication(QObject *parent = 0);

    QUdpSocket *m_udpSocket;
    QTcpServer *m_tcpServer;
    QTcpSocket *m_tcpSocket;
    int m_udpPort;
    QString m_localAddress;
    QString m_localMAC;
    int m_tcpPort;
    QTimer *m_udpTimer;
    QMap<QString, QTcpSocket*> *m_tcpSocketMap;

    void processDatagram(QByteArray &datagram);
    void handshakeACK(QTcpSocket *socket, QJsonObject jsonObject);
    void handshakeSYN(QTcpSocket *socket, QJsonObject jsonObject);
    void infoReceived(QJsonObject jsonObject);
    void answerFromReader(QJsonObject dataObj);
    void sendAckUnknown(QTcpSocket *socket, QJsonObject originalMessage, QString errorInfo);
    void ackUnknownReceived(QJsonObject jsonObject);
    void ackNewConfig(QJsonObject jsonObject);

public:
    static NetworkCommunication *instance();
    void startBroadcast();
    void stopBroadcast();
    void sendData(QTcpSocket *socket, const QString &type, const QJsonObject &data);
    void connectToRasp(const QString &ip);
    void getConfigFromRasp();
    void sendNewConfigToRasp(const QJsonObject &json);
    void sendNewCommandToReader(const QString &command);
    void closeTCPConnection();

private slots:
    void tcpDataAvailable();
    void tcpSocketError(const QAbstractSocket::SocketError socketError);
    void newConnection();
    void tcpDisconnected();
    void sendDatagram();
signals:
    void raspDisconnected(QString ipAddress);
    void newRaspFound(QVariantMap raspInfo);
    void currentConfigFromRasp(QJsonObject json);
    void newRFIDMontiorAnswer(QString answer);
    void newReaderAnswer(QString answer);
    void connectionEstablished();
    void connectionFailed();
    void currentConfigStatusFromRasp(QJsonObject obj);
};

#endif // NETWORKCOMMUNICATION_H
