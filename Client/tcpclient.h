#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QAbstractSocket>

class QTcpSocket;

class TcpClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = 0);

signals:

public slots:
    void start();

    void readyRead();
    void sendRandomData();
    void sendRandomDataNewDataPacket();
    void sendRandomClientPacket();

    void connected();
    void disconnected();

    void handleStateChanged(QAbstractSocket::SocketState socketState);
    void handleError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *m_socket;

};

#endif // TCPCLIENT_H
