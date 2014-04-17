#include <QJsonDocument>
#include <QJsonParseError>
#include <QDebug>
#include <QNetworkInterface>
#include <QMapIterator>
#include <QDateTime>

#include "networkcommunication.h"
#include "settings.h"
#include "../gui/systemmessageswidget.h"

NetworkCommunication::NetworkCommunication(QObject *parent) :
    QObject(parent)
{
    m_tcpSocketMap = new QMap<QString, QTcpSocket *>();

    m_udpPort = 9999;
    m_udpSocket = new QUdpSocket(this);
    m_udpTimer = new QTimer(this);
    m_udpTimer->setInterval(1000);
    connect(m_udpTimer, SIGNAL(timeout()), this, SLOT(sendDatagram()));

    m_tcpServer = new QTcpServer(this);
    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));

    m_tcpSocket = 0;
    m_tcpPort = 8888;

}

void NetworkCommunication::handshakeACK(QTcpSocket *socket, QJsonObject jsonObject)
{

    if(socket->isOpen()){

        QTimer * timeout = socket->findChild<QTimer *>();
        timeout->stop();

        m_tcpSocketMap->insert(socket->peerAddress().toString(), socket);
        QVariantMap map;
        map.insert("ipaddress",socket->peerAddress().toString());
        map.insert("macaddress", jsonObject.value("macaddress").toString());
        map.insert("name",jsonObject.value("name").toString());

        emit newRaspFound(map);
    }else{
        qDebug() << "Socket not open.";
    }


}

void NetworkCommunication::handshakeSYN(QTcpSocket *socket, QJsonObject jsonObject)
{
    if(socket->isOpen()){
        sendData(socket, "ACK-SYN", jsonObject);
    }else{
        qDebug() << "Socket not open.";
    }
}

void NetworkCommunication::infoReceived(QJsonObject jsonObject)
{
    emit currentConfigFromRasp(jsonObject);
}

void NetworkCommunication::answerFromReader(QJsonObject dataObj)
{
    emit newReaderAnswer(dataObj.value("response").toString());
}

void NetworkCommunication::sendAckUnknown(QTcpSocket *socket, QJsonObject originalMessage, QString errorInfo)
{
    QJsonObject obj;
    obj.insert("unknownmessage",originalMessage);
    obj.insert("errorinfo",errorInfo);
    sendData(socket,"ACK-UNKNOWN", obj);
}

void NetworkCommunication::ackUnknownReceived(QJsonObject jsonObject)
{
    qDebug() << "ACK-UNKNOWN RECEIVED.";
    qDebug() << "Error info: " << jsonObject.value("errorinfo").toString();
    qDebug() << "From this our message early sent: " << jsonObject.value("unknownmessage").toObject().toVariantMap();
}

void NetworkCommunication::ackNewConfig(QJsonObject jsonObject)
{
    emit currentConfigStatusFromRasp(jsonObject);
}

void NetworkCommunication::sendDatagram()
{
    QString string("{\"ipaddress\": \"%1\" ,"
                   " \"port\" : %2 "
                   " } ");

    QByteArray datagram(string.arg(m_localAddress).arg(m_tcpPort).toUtf8());

    if(m_udpSocket->writeDatagram(datagram.data(), datagram.size(), QHostAddress::Broadcast, m_udpPort) == -1){
        SystemMessagesWidget::instance()->writeMessage("Error: cannot write datagram. Error Message: " + m_udpSocket->errorString(),
                                                       SystemMessagesWidget::KError,
                                                       SystemMessagesWidget::KDialogAndLog);
        m_udpTimer->stop();
    }
}

NetworkCommunication *NetworkCommunication::instance()
{
    static NetworkCommunication *singleton = 0;
    if(!singleton)
        singleton = new NetworkCommunication;
    return singleton;
}

void NetworkCommunication::startBroadcast()
{
    if(m_tcpServer->listen(QHostAddress::Any)){
        m_tcpPort = m_tcpServer->serverPort();

        QList<QNetworkInterface> netInterfaces = QNetworkInterface::allInterfaces();

        QHostAddress address;
        QString mac;

        foreach (QNetworkInterface interface, netInterfaces) {

            QList<QHostAddress> ipAddressesList = interface.allAddresses();

            foreach (QHostAddress var, ipAddressesList) {
                if(!var.isLoopback()){
                    mac = interface.hardwareAddress();
                    address = var;
                    break;
                }
            }
            if( ! address.isNull()){
                m_localAddress = address.toString();
                m_localMAC = mac;

                m_udpTimer->start();

                SystemMessagesWidget::instance()->writeMessage(QString("Waiting for connections on %1 %2").arg(m_localAddress).arg(m_tcpPort));
                break;

            }else{
                SystemMessagesWidget::instance()->writeMessage("Error. cannot find a valid address to start the server.");
            }
        }

    }else{
        qDebug() << "Error. cannot start listening.";
    }


}

void NetworkCommunication::stopBroadcast()
{
    m_tcpServer->close();
    m_udpTimer->stop();

    QMapIterator<QString, QTcpSocket *> i(*m_tcpSocketMap);
    while (i.hasNext()) {
        i.next();

        i.value()->close();
    }

    SystemMessagesWidget::instance()->writeMessage("Stoped searching for rasps.");
}

void NetworkCommunication::sendData(QTcpSocket *socket, const QString &type, const QJsonObject &data)
{
    if(socket && socket->isOpen()){
        QJsonObject rootObj;

        rootObj.insert("type", QJsonValue(type));
        rootObj.insert("data",data);
        rootObj.insert("datetime",QDateTime::currentDateTime().toString(Qt::ISODate));

        QJsonDocument doc;
        doc.setObject(rootObj);

        QByteArray package(doc.toJson());

        //PACKAGE SIZE
        QByteArray packageSize;
        QString packageSizeStr(QString::number(package.size()));
        packageSize.fill('0', sizeof(quint64) - packageSizeStr.size());
        packageSize.append(packageSizeStr);

        qDebug() << "Message sent: SIZE: " << packageSize << " DATA: " << package;

        if(socket->write(packageSize) == sizeof(quint64)){
//            socket->flush();
            if(socket->write(package) == package.size())
                socket->flush();
            else
                qDebug() << "FAIL";
        }else
            qDebug() << "FAIL";

    }else{
        qDebug() << "Cannot send data. The socket is closed.";
    }

}

void NetworkCommunication::connectToRasp(const QString &ip)
{
    if(m_tcpSocketMap->contains(ip) && m_tcpSocketMap->value(ip)->isOpen()){

        //elimina outros sockets

        m_tcpSocket = m_tcpSocketMap->value(ip);
        m_tcpSocketMap->remove(ip);
        stopBroadcast();

        emit connectionEstablished();
    }else{
        emit connectionFailed();
    }
}

void NetworkCommunication::getConfigFromRasp()
{
    sendData(m_tcpSocket, "GET-CONFIG", QJsonObject());
}

void NetworkCommunication::sendNewConfigToRasp(const QJsonObject &json)
{
    sendData(m_tcpSocket, "NEW-CONFIG", json);
}

void NetworkCommunication::sendNewCommandToReader(const QString &command)
{
    QJsonObject obj;
    obj.insert("reader", QString("A1"));
    obj.insert("command", command);
    NetworkCommunication::instance()->sendData(m_tcpSocket,"READER-COMMAND",obj);
}

void NetworkCommunication::closeTCPConnection()
{
    if(m_tcpSocket)
        m_tcpSocket->disconnectFromHost();
    else
        qDebug() << "Connection already closed.";
}

void NetworkCommunication::tcpDataAvailable()
{
    qDebug() << "new data arrived.";
    QTcpSocket *socket = (QTcpSocket *) QObject::sender();

    static bool waitingForPackage = false;
    static quint64 sizeOfPackage = 0;

    if( ! waitingForPackage){
        if((quint64)socket->bytesAvailable() < sizeof(quint64))
            return;

        QString packageSizeStr(socket->read(sizeof(quint64)));
        sizeOfPackage = packageSizeStr.toULongLong();
        waitingForPackage = true;
    }
    if((quint64)socket->bytesAvailable() >=  sizeOfPackage){
        QByteArray package(socket->read(sizeOfPackage));

        QJsonDocument doc(QJsonDocument::fromJson(package));
        if(!doc.isNull()){
            QJsonObject rootObj(doc.object());

            qDebug() << "New tcp data: " <<rootObj.toVariantMap();

            QString type(rootObj.value("type").toString());
            QJsonObject dataObj = rootObj.value("data").toObject();
            if(type == "SYN"){
                handshakeSYN(socket, dataObj);
            }else if(type == "ACK"){
                handshakeACK(socket, dataObj);
            }else if(type == "CONFIG"){
                infoReceived(dataObj);
            }else if(type == "READER-RESPONSE"){
                answerFromReader(dataObj);
            }else if(type == "ACK-UNKNOWN"){
                ackUnknownReceived(dataObj);
            }else if(type == "ACK-NEW-CONFIG"){
                ackNewConfig(dataObj);
            }else{
                qDebug() << "Data type invalid.";
                sendAckUnknown(socket, rootObj, "The 'type' is unknown.");
            }
        }else{
            qDebug() << "Invalid json.";
            sendAckUnknown(socket, QJsonObject(), "The network package could not be parsed"
                       " to a QJsonDocument. The json is invalid.");
        }

        waitingForPackage = false;
        sizeOfPackage = 0;
    }
}

void NetworkCommunication::tcpSocketError(const QAbstractSocket::SocketError socketError)
{
    QTcpSocket *socket = (QTcpSocket *) QObject::sender();

    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        SystemMessagesWidget::instance()->writeMessage("The host closed the connection.",
                                                       SystemMessagesWidget::KError);
        break;
    case QAbstractSocket::HostNotFoundError:
        SystemMessagesWidget::instance()->writeMessage("The host was not found. Please check the "
                                                       "host name and port settings.",
                                                       SystemMessagesWidget::KError);
        break;
    case QAbstractSocket::ConnectionRefusedError:
        SystemMessagesWidget::instance()->writeMessage("The connection was refused by the peer. "
                                                       "Make sure the application on the host is running, "
                                                       "and check that the host name and port "
                                                       "settings are correct.",
                                                       SystemMessagesWidget::KError);
        break;
    default:
        if(socket)
            qDebug() << QString("The following error occurred: %1.").arg(socket->errorString());
        else
            qDebug() << "m_tcpSocket does not exists anymore.";
    }
    emit connectionFailed();
}

void NetworkCommunication::newConnection()
{
    QTcpSocket *socket = m_tcpServer->nextPendingConnection();

    QTimer *timeout = new QTimer(socket);
    timeout->setInterval(5000);
    timeout->setSingleShot(true);

    connect(socket,
            SIGNAL(readyRead()),
            this,
            SLOT(tcpDataAvailable()));
    connect(socket,
            SIGNAL(error(QAbstractSocket::SocketError)),
            this,
            SLOT(tcpSocketError(QAbstractSocket::SocketError)));
    connect(socket,
            SIGNAL(disconnected()),
            this,
            SLOT(tcpDisconnected()));
    connect(timeout,
            SIGNAL(timeout()),
            socket,
            SLOT(deleteLater()));

    SystemMessagesWidget::instance()->writeMessage("New connection arrived.");

    timeout->start();

}

void NetworkCommunication::tcpDisconnected()
{
    QTcpSocket *socket = (QTcpSocket *) QObject::sender();
    QString ip = socket->peerAddress().toString();
    if(socket){
        qDebug() << "Deleting the socket";
        SystemMessagesWidget::instance()->writeMessage("Disconnected from " + ip);
        if(m_tcpSocketMap->contains(ip)){
            emit raspDisconnected(socket->peerAddress().toString());
            m_tcpSocketMap->remove(socket->peerAddress().toString());
        }else if(m_tcpSocket){
            m_tcpSocket = 0;
        }else{
            qDebug() << "Timeout..." <<  ip;
        }
        socket->deleteLater();
    }else{
        qDebug() << "socket does not exists anymore. grr..";
    }
}
