#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>

class QTcpServer;

class TCPServer : public QObject
{
    Q_OBJECT
public:
    explicit TCPServer(QObject *parent = 0);

    void start();

signals:

public slots:
    void newConnection();
    void broadcast();
    void onDisconnected();
    void readyRead();
private:
    QTcpServer *m_server;

};

#endif // TCPSERVER_H
