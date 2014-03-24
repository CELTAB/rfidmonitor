#ifndef TABRASPBERRYCONNECTION_H
#define TABRASPBERRYCONNECTION_H

#include <QWidget>
#include <QUdpSocket>
#include <QByteArray>

namespace Ui {
class TabRaspberryConnection;
}

class TabRaspberryConnection : public QWidget
{
    Q_OBJECT

public:
    explicit TabRaspberryConnection(QWidget *parent = 0);
    ~TabRaspberryConnection();
    void writeToLog(QString text);

private:
    Ui::TabRaspberryConnection *ui;
    QUdpSocket *m_udpSocket;
    int m_udpPort;
    void startListening();
    void stopListening();
    void parseDatagramToJson(QByteArray *datagram);

public slots:
    void processDatagram();
    void btSearchClicked();
    void btStopSearchClicked();
};

#endif // TABRASPBERRYCONNECTION_H
