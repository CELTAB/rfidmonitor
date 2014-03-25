#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>

#include "networkcommunication.h"
#include "settings.h"
#include "../gui/systemmessageswidget.h"

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

void NetworkCommunication::handshake(QByteArray byteArray)
{
    QString str(byteArray);
    qDebug() << "Received from Rasp: " << str;
    if(str.compare("I' am fine.") == 0)
        qDebug() << "Ok! Answer correctly received from rasp. We can proced.";
    else
        qDebug() << "We could not understand what rasp said.";
}

NetworkCommunication *NetworkCommunication::instance()
{
    static NetworkCommunication *singleton = 0;
    if(!singleton)
        singleton = new NetworkCommunication;
    return singleton;
}

void NetworkCommunication::startListeningBroadcast()
{
    if(m_udpSocket->bind(m_udpPort,QUdpSocket::DontShareAddress)){
        SystemMessagesWidget::instance()->writeMessage("Searching for Rasps...");

    }else{
        SystemMessagesWidget::instance()->writeMessage("Failed start searching for Rasps...",
                                                       SystemMessagesWidget::KError,
                                                       SystemMessagesWidget::KDialogAndLog);
    }
}

void NetworkCommunication::stopListeningBroadcast()
{
    m_udpSocket->close();

    SystemMessagesWidget::instance()->writeMessage("Stoped searching for rasps.");
}

void NetworkCommunication::sendData(const QByteArray &data, const Settings::TcpDataType &dataType)
{
    //WRITE USING THE PROTOCOL...NOT WORKING YET
}

bool NetworkCommunication::connectToRasp(QString ip, int port)
{
    m_tcpSocket->connectToHost(QHostAddress(ip), port);

    QByteArray package;

    //DATA TYPE
    QByteArray dataType;
    QString dataTypeStr(QString::number(Settings::KHandshake));
    dataType.fill('0', sizeof(quint32) - dataTypeStr.size());
    dataType.append(dataTypeStr);
    package.append(dataType);
    //DATA
    package.append(QByteArray("How are you?"));

    //PACKAGE SIZE
    QByteArray packageSize;
    QString packageSizeStr(QString::number(package.size()));
    packageSize.fill('0', sizeof(quint64) - packageSizeStr.size());
    packageSize.append(packageSizeStr);

    m_tcpSocket->write(packageSize);
    m_tcpSocket->write(package);
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
    while(m_udpSocket->hasPendingDatagrams()){

        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(datagram.data(), datagram.size());
        processDatagram(datagram);
    }
}

void NetworkCommunication::tcpDataAvailable()
{
    static bool waitingForPackage = false;
    static quint64 sizeOfPackage = 0;

    qDebug() << "dataarrived. Bytes available: " << m_tcpSocket->bytesAvailable();
    if( ! waitingForPackage){
        qDebug() << "not waiting for package";

        if((quint64)m_tcpSocket->bytesAvailable() < sizeof(quint64))
            return;

        QString packageSizeStr(m_tcpSocket->read(sizeof(quint64)));

        sizeOfPackage = packageSizeStr.toULongLong();
        qDebug() << "Size of comming package: " << sizeOfPackage;
        waitingForPackage = true;
        qDebug() << "waiting for package";
    }
    qDebug() << "Bytes available after checking package size: " << m_tcpSocket->bytesAvailable();
    if((quint64)m_tcpSocket->bytesAvailable() >=  sizeOfPackage){

        qDebug() << "enought bytes for the package";

        QByteArray package(m_tcpSocket->read(sizeOfPackage));
        qDebug() << "Before handshake. package size: " << package.size();
        qDebug() << "Before handshake. package data: " << package.data();
        QString dataTypeStr(package.left(sizeof(quint32)));
        Settings::TcpDataType dataType = (Settings::TcpDataType) dataTypeStr.toInt();
        switch (dataType) {
        case Settings::KHandshake:
            handshake(package.right(package.size() - sizeof(quint32)));
            break;
        default:
            qDebug() << "Data type invalid.";
            break;
        }
        waitingForPackage = false;
        sizeOfPackage = 0;
    }
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
