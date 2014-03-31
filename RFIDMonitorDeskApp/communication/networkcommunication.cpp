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
    if(str.compare("I' am fine.") == 0)
        emit connectionEstablished();
    else{
        SystemMessagesWidget::instance()->writeMessage("Failed to use app communication protocol.",
                                                       SystemMessagesWidget::KError,
                                                       SystemMessagesWidget::KDialogAndLog);
        emit connectionFailed();
    }

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
        SystemMessagesWidget::instance()->writeMessage("Failed start searching for Rasps... Cannot bind UDP port. Is open a other instance of this application?",
                                                       SystemMessagesWidget::KError,
                                                       SystemMessagesWidget::KDialogAndLog);
    }
}

void NetworkCommunication::stopListeningBroadcast()
{
    m_udpSocket->close();

    SystemMessagesWidget::instance()->writeMessage("Stoped searching for rasps.");
}

void NetworkCommunication::sendData(const QByteArray &data, const Settings::TcpDataType &type)
{
    QByteArray package;

    //DATA TYPE
    QByteArray dataType;
    QString dataTypeStr(QString::number(type));
    dataType.fill('0', sizeof(quint32) - dataTypeStr.size());
    dataType.append(dataTypeStr);
    package.append(dataType);
    //DATA
    package.append(data);

    //PACKAGE SIZE
    QByteArray packageSize;
    QString packageSizeStr(QString::number(package.size()));
    packageSize.fill('0', sizeof(quint64) - packageSizeStr.size());
    packageSize.append(packageSizeStr);

    m_tcpSocket->write(packageSize);
    m_tcpSocket->write(package);
    m_tcpSocket->flush();
}

void NetworkCommunication::connectToRasp(const QString &ip, const int &port)
{
    m_tcpSocket->connectToHost(QHostAddress(ip), port);
    int limitWaitTime = 10000;
    SystemMessagesWidget::instance()->writeMessage(QString("Trying to connect to rasp. Waiting a limit of %1 seconds...").arg(limitWaitTime / 1000));
    bool connected = m_tcpSocket->waitForConnected(limitWaitTime);

    if(connected)
        sendData(QString("How are you?").toLocal8Bit(), Settings::KHandshake);
}

void NetworkCommunication::triggerToGetCurrentConfigFromRasp()
{
    QString instruction("give-me-your-current-config");
    sendData(instruction.toLocal8Bit(), Settings::KInstruction);
}

void NetworkCommunication::sendNewConfigToRasp(const QByteArray &json)
{
    sendData(json, Settings::KConfiguration);
}

void NetworkCommunication::sendNewCommandToReader(const QString &command)
{
    NetworkCommunication::instance()->sendData(command.toLocal8Bit(), Settings::KNewReaderCommand);
}

void NetworkCommunication::closeTCPConnection()
{
    m_tcpSocket->disconnectFromHost();
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

    if( ! waitingForPackage){
        if((quint64)m_tcpSocket->bytesAvailable() < sizeof(quint64))
            return;

        QString packageSizeStr(m_tcpSocket->read(sizeof(quint64)));
        sizeOfPackage = packageSizeStr.toULongLong();
        waitingForPackage = true;
    }
    if((quint64)m_tcpSocket->bytesAvailable() >=  sizeOfPackage){
        QByteArray package(m_tcpSocket->read(sizeOfPackage));
        QString dataTypeStr(package.left(sizeof(quint32)));
        Settings::TcpDataType dataType = (Settings::TcpDataType) dataTypeStr.toInt();
        switch (dataType) {
        case Settings::KHandshake:
            handshake(package.right(package.size() - sizeof(quint32)));
            break;
        case Settings::KNewReaderAnswer:
            emit newReaderAnswer(package.right(package.size() - sizeof(quint32)));
            break;
        default:
            qDebug() << "Data type invalid.";
            break;
        }
        waitingForPackage = false;
        sizeOfPackage = 0;
    }
}

void NetworkCommunication::tcpSocketError(const QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        SystemMessagesWidget::instance()->writeMessage("The host closed the connection.",
                                                       SystemMessagesWidget::KError,
                                                       SystemMessagesWidget::KDialogAndLog);
        break;
    case QAbstractSocket::HostNotFoundError:
        SystemMessagesWidget::instance()->writeMessage("The host was not found. Please check the "
                                                       "host name and port settings.",
                                                       SystemMessagesWidget::KError,
                                                       SystemMessagesWidget::KDialogAndLog);
        break;
    case QAbstractSocket::ConnectionRefusedError:
        SystemMessagesWidget::instance()->writeMessage("The connection was refused by the peer. "
                                                       "Make sure the application on the host is running, "
                                                       "and check that the host name and port "
                                                       "settings are correct.",
                                                       SystemMessagesWidget::KError,
                                                       SystemMessagesWidget::KDialogAndLog);
        break;
    default:
        SystemMessagesWidget::instance()->writeMessage(QString("The following error occurred: %1.").arg(m_tcpSocket->errorString()),
                                                       SystemMessagesWidget::KError,
                                                       SystemMessagesWidget::KDialogAndLog);
    }
    emit connectionFailed();
}
