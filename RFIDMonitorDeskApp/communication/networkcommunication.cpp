#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>

#include "networkcommunication.h"
#include "settings.h"

NetworkCommunication::NetworkCommunication(QObject *parent) :
    QObject(parent)
{
    m_udpPort = 9999;
    m_udpSocket = new QUdpSocket(this);
    connect(m_udpSocket, SIGNAL(readyRead()),this,SLOT(udpDataAvailable()));
    m_tcpPort = 8888;
    m_tcpSocket = new QTcpSocket(this);
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(tcpDataAvailable()));
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(tcpSocketError(QAbstractSocket::SocketError)));
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(tcpSocketDisconnected()));
}

void NetworkCommunication::processDatagram(QByteArray &datagram)
{
    QJsonParseError err;
    QJsonDocument doc(QJsonDocument::fromJson(datagram,&err));

    if( ! doc.isNull()){

        //VERIFY IF DATAGRAM HAVE THE NECESSARY INFO?

        emit newRaspFound(doc.object().toVariantMap());
    }else{
        qDebug() << "invalid conversion from datagram data to json. Data received: " << datagram;
    }
}

NetworkCommunication *NetworkCommunication::instance()
{
    static NetworkCommunication *singleton = 0;
    if(!singleton)
        singleton = new NetworkCommunication(qApp);
    return singleton;
}

void NetworkCommunication::startListeningBroadcast()
{
    if(m_udpSocket->bind(m_udpPort,QUdpSocket::DontShareAddress)){
        //        writeToLog(QString(tr("Listening for Raspberry Pi's on port %1.")).arg(m_udpPort));
        qDebug() << "listening";
    }else{
        //        writeToLog(QString(tr("Failed to start listening. Cannot bind to port %1")).arg(m_udpPort));
    }
}

void NetworkCommunication::stopListeningBroadcast()
{
    m_udpSocket->close();
    //        writeToLog(QString(tr("Stopped listening.")));
    qDebug() << "stoped to listen";
}

void NetworkCommunication::sendData(const QByteArray &data, const Settings::TcpDataType &dataType)
{
    //WRITE USING THE PROTOCOL...NOT WORKING YET
}

bool NetworkCommunication::connectToRasp(QString ip, int port)
{
    m_tcpSocket->connectToHost(QHostAddress(ip), port);

//    QByteArray package;

//    //DATA TYPE
//    QByteArray dataType((int)Settings::KHandshake);
//    dataType.resize(sizeof(quint8));
//    package.append(dataType);

//    //DATA
//    package.append(QByteArray("How are you?"));

//    //PACKAGE SIZE
//    QByteArray packageSize(package.size());
//    packageSize.resize(sizeof(quint64));

//    //NAO VAI FUNCIONAR PQ O RESIZE VAI ENCHER DE LIXO O RESTO.

//    qDebug() << "Package size: " << packageSize;

//    m_tcpSocket->write(packageSize);
//    m_tcpSocket->write(package);
    return true;
}

void NetworkCommunication::triggerToGetCurrentConfigFromRasp()
{
    QString instruction("give-me-your-current-config");
    sendData(instruction.toLocal8Bit(), Settings::KInstruction);
}

void NetworkCommunication::sendNewConfigToRasp(QByteArray json)
{
    sendData(json, Settings::KConfiguration);
}

void NetworkCommunication::tcpSocketDisconnected()
{
    emit raspDisconnected();
}

void NetworkCommunication::udpDataAvailable()
{
    qDebug() << "data available";
    while(m_udpSocket->hasPendingDatagrams()){

        qDebug() << "reading new datagram";
        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(datagram.data(), datagram.size());
        processDatagram(datagram);
    }
}

void NetworkCommunication::tcpDataAvailable()
{
//    if(dataType == Settings::KRFIDMonitorAnswer){
//        emit newRFIDMonitorAnswer(QString);
//    }
    //    if(dataType == Settings::KConfiguration){
    //        emit currentConfigFromRasp(QbyteArray);
    //    }
}

void NetworkCommunication::tcpSocketError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
            break;
        case QAbstractSocket::HostNotFoundError:
            qDebug() << "The host was not found. Please check the "
                                        "host name and port settings.";
            break;
        case QAbstractSocket::ConnectionRefusedError:
            qDebug() << "The connection was refused by the peer. "
                                        "Make sure the fortune server is running, "
                                        "and check that the host name and port "
                                        "settings are correct.";
            break;
        default:
            qDebug() << QString("The following error occurred: %1.")
                                     .arg(m_tcpSocket->errorString());
        }
}
