#include <QTime>
#include <QStandardItemModel>
#include <QStandardItem>

#include "tabraspberryconnection.h"
#include "ui_tabraspberryconnection.h"
#include "raspconfigurationjson.h"

TabRaspberryConnection::TabRaspberryConnection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabRaspberryConnection)
{
    ui->setupUi(this);
    m_udpPort = 9999;
    m_udpSocket = new QUdpSocket(this);

    //MEGA TEST
    QStandardItemModel *model = new QStandardItemModel(this);
    ui->lvDevicesFound->setModel(model);
    QStandardItem *item = new QStandardItem();
    RaspConfigurationJson *json = new RaspConfigurationJson;
    item->setData("oi", Qt::DisplayRole);
    item->setData((char *)json, Qt::UserRole);
    model->appendRow(item);
    //END OF MEGA TEST

    connect(ui->btSearch, SIGNAL(clicked()), this, SLOT(btSearchClicked()));
    connect(ui->btStopSearch, SIGNAL(clicked()), this, SLOT(btStopSearchClicked()));
    connect(m_udpSocket, SIGNAL(readyRead()), this, SLOT(processDatagram()));
}

TabRaspberryConnection::~TabRaspberryConnection()
{
    delete ui;
}

void TabRaspberryConnection::writeToLog(QString text)
{
    ui->teLogs->append("[" + QTime::currentTime().toString() + "] " + text);
}

void TabRaspberryConnection::startListening()
{
    if(m_udpSocket->bind(m_udpPort,QUdpSocket::DontShareAddress)){
        writeToLog(QString(tr("Listening for Raspberry Pi's on port %1.")).arg(m_udpPort));
    }else{
        writeToLog(QString(tr("Failed to start listening. Cannot bind to port %1")).arg(m_udpPort));
    }
}

void TabRaspberryConnection::stopListening()
{
    if(m_udpSocket->isOpen()){
        m_udpSocket->close();
        writeToLog(QString(tr("Stopped listening.")));
    }
}

void TabRaspberryConnection::parseDatagramToJson(QByteArray *datagram)
{

}

void TabRaspberryConnection::processDatagram()
{
    while(m_udpSocket->hasPendingDatagrams()){
        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(datagram.data(), datagram.size());
        parseDatagramToJson(&datagram);
    }
}

void TabRaspberryConnection::btSearchClicked()
{
    startListening();
}

void TabRaspberryConnection::btStopSearchClicked()
{
    stopListening();
}
