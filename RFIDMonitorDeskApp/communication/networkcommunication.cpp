/****************************************************************************
**
** www.celtab.org.br
**
** Copyright (C) 2013
**                     Gustavo Valiati <gustavovaliati@gmail.com>
**                     Luis Valdes <luisvaldes88@gmail.com>
**                     Thiago R. M. Bitencourt <thiago.mbitencourt@gmail.com>
**
** This file is part of the RFIDMonitor project
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; version 2
** of the License.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**
****************************************************************************/

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
        SystemMessagesWidget::instance()->writeMessage(
                    tr("Socket is not open."),
                    SystemMessagesWidget::KDebug,
                    SystemMessagesWidget::KOnlyLogfile
                    );
    }


}

void NetworkCommunication::handshakeSYN(QTcpSocket *socket, QJsonObject jsonObject)
{
    if(socket->isOpen()){
        sendData(socket, "ACK-SYN", jsonObject);
    }else{
        SystemMessagesWidget::instance()->writeMessage(
                    tr("Socket is not open."),
                    SystemMessagesWidget::KDebug,
                    SystemMessagesWidget::KOnlyLogfile
                    );
    }
}

void NetworkCommunication::raspConfigReceived(QJsonObject jsonObject)
{
    emit currentConfigFromRasp(jsonObject);
}

void NetworkCommunication::answerFromReader(QJsonObject dataObj)
{
    QString response(dataObj.value("response").toString());
    emit newReaderAnswer(response);
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
    SystemMessagesWidget::instance()->writeMessage(
                tr("ACK-UNKNOWN Received. ERROR INFO [%1] ORIGINAL MESSAGE [%2]")
                .arg(jsonObject.value("errorinfo").toString())
                .arg(QString(QJsonDocument(jsonObject.value("unknownmessage").toObject()).toJson())),
                SystemMessagesWidget::KDebug,
                SystemMessagesWidget::KOnlyLogfile
                );
}

void NetworkCommunication::ackNewConfig(QJsonObject jsonObject)
{
    emit currentConfigStatusFromRasp(jsonObject);
}

void NetworkCommunication::sendDatagram()
{
    QString string("{"
                   " \"ipaddress\": \"%1\" ,"
                   " \"port\" : %2 "
                   "}");

    QByteArray datagram(string.arg(m_localAddress).arg(m_tcpPort).toUtf8());

    if(m_udpSocket->writeDatagram(datagram.data(), datagram.size(), QHostAddress::Broadcast, m_udpPort) == -1){
        SystemMessagesWidget::instance()->writeMessage("Cannot write datagram. Message: " + m_udpSocket->errorString(),
                                                       SystemMessagesWidget::KError,
                                                       SystemMessagesWidget::KDialogAndTextbox);
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

        foreach (QNetworkInterface interface, netInterfaces) {

            QList<QHostAddress> ipAddressesList = interface.allAddresses();

            foreach (QHostAddress var, ipAddressesList) {
                if(!var.isLoopback()){
                    address = var;
                    break;
                }
            }
            if( ! address.isNull()){
                m_localAddress = address.toString();

                m_udpTimer->start();

                SystemMessagesWidget::instance()->writeMessage(QString(tr("Waiting for connections on %1:%2")).arg(m_localAddress).arg(m_tcpPort));
                break;

            }else{
                SystemMessagesWidget::instance()->writeMessage(tr("Cannot find a valid address to start the server."),
                                                               SystemMessagesWidget::KError,
                                                               SystemMessagesWidget::KDialogAndTextbox);
            }
        }

    }else{
        SystemMessagesWidget::instance()->writeMessage(
                    tr("Cannot start searching."),
                    SystemMessagesWidget::KError,
                    SystemMessagesWidget::KDialogAndTextbox
                    );
    }


}

void NetworkCommunication::stopBroadcast()
{
    m_tcpServer->close();
    m_udpTimer->stop();

    /*Close the remaining connections in the map.
     * If the user selected a host to maintain connection, it was removed from
     * this map in connectToRasp(), and by this it will not be closed here.*/
    QMapIterator<QString, QTcpSocket *> i(*m_tcpSocketMap);
    while (i.hasNext()) {
        i.next();

        i.value()->close();
    }

    SystemMessagesWidget::instance()->writeMessage(tr("Stopped searching for new rasps."));
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
        SystemMessagesWidget::instance()->writeMessage(
                    tr("Message sent -> SIZE [%1] MESSAGE[%2]")
                    .arg(packageSizeStr)
                    .arg(QString(package)),
                    SystemMessagesWidget::KDebug,
                    SystemMessagesWidget::KOnlyLogfile
                    );

        if(socket->write(packageSize) == sizeof(quint64)){
            //            socket->flush();
            if(socket->write(package) == package.size())
                socket->flush();
            else{

                SystemMessagesWidget::instance()->writeMessage(tr("Failed to write bytes to the socket!!!"),
                                                               SystemMessagesWidget::KError,
                                                               SystemMessagesWidget::KDialogAndTextbox);
            }
        }else{

            SystemMessagesWidget::instance()->writeMessage(tr("Failed to write bytes to the socket!!!"),
                                                           SystemMessagesWidget::KError,
                                                           SystemMessagesWidget::KDialogAndTextbox);
        }

    }else{
        SystemMessagesWidget::instance()->writeMessage(
                    tr("Cannot send data. The socket is closed."),
                    SystemMessagesWidget::KError,
                    SystemMessagesWidget::KDialogAndTextbox
                    );
    }

}

void NetworkCommunication::connectToRasp(const QString &ip)
{
    if(m_tcpSocketMap->contains(ip) && m_tcpSocketMap->value(ip)->isOpen()){

        // Holds the connection to the host selected by the user.
        m_tcpSocket = m_tcpSocketMap->value(ip);
        // And remove it from the map.
        m_tcpSocketMap->remove(ip);
        // The broadcast will stop, and all remaining connection in the map
        // will be closed.
        stopBroadcast();

        SystemMessagesWidget::instance()->writeMessage(tr("Successfully connect to rasp."));

        // Main connection successfully defined.
        emit connectionEstablished();
    }else{
        SystemMessagesWidget::instance()->writeMessage(tr("Failed to connect to rasp."),
                                                       SystemMessagesWidget::KError,
                                                       SystemMessagesWidget::KDialogAndTextbox);
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
    /* This slot is called when the GUI button "Close Connection" is clicked.
     * If the connection was current main connection is lost and closed,
     * the button can still be clicked to close the window, and for this its
     * needed to check if the connection is still there or not.*/
    if(m_tcpSocket)
        m_tcpSocket->disconnectFromHost();
}

void NetworkCommunication::sendFullRead(bool full)
{
    QJsonObject dataObj;
    dataObj.insert("full", QJsonValue(full));

    sendData(m_tcpSocket, "FULL-READ", dataObj);
}

void NetworkCommunication::tcpDataAvailable()
{
//    SystemMessagesWidget::instance()->writeMessage(
//                tr("New data arrived."),
//                SystemMessagesWidget::KDebug,
//                SystemMessagesWidget::KOnlyLogfile
//                );
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
//            SystemMessagesWidget::instance()->writeMessage(
//                        tr("New valid json object: %1")
//                        .arg(QString(QJsonDocument(rootObj).toJson())),
//                        SystemMessagesWidget::KDebug,
//                        SystemMessagesWidget::KOnlyLogfile
//                        );

            QString type(rootObj.value("type").toString());
            QJsonObject dataObj = rootObj.value("data").toObject();
            if(type == "SYN"){
                handshakeSYN(socket, dataObj);
            }else if(type == "ACK"){
                handshakeACK(socket, dataObj);
            }else if(type == "CONFIG"){
                raspConfigReceived(dataObj);
            }else if(type == "READER-RESPONSE"){
                answerFromReader(dataObj);
            }else if(type == "ACK-UNKNOWN"){
                ackUnknownReceived(dataObj);
            }else if(type == "ACK-NEW-CONFIG"){
                ackNewConfig(dataObj);
            }else{
                SystemMessagesWidget::instance()->writeMessage(
                            tr("Data type invalid."),
                            SystemMessagesWidget::KDebug,
                            SystemMessagesWidget::KOnlyLogfile
                            );
                sendAckUnknown(socket, rootObj, "The 'type' is unknown.");
            }
        }else{
            SystemMessagesWidget::instance()->writeMessage(
                        tr("Invalid json. DATA [%1]").arg(QString(package)),
                        SystemMessagesWidget::KError,
                        SystemMessagesWidget::KOnlyLogfile
                        );
            sendAckUnknown(socket, QJsonObject(), "The network package could not be parsed"
                           " to a QJsonDocument. The json is invalid.");
        }

        waitingForPackage = false;
        sizeOfPackage = 0;
    }
}

void NetworkCommunication::tcpSocketError(const QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        SystemMessagesWidget::instance()->writeMessage(
                    tr("The remote device closed the connection."),
                    SystemMessagesWidget::KError,
                    SystemMessagesWidget::KDialogAndTextbox);
        break;
    case QAbstractSocket::HostNotFoundError:
        SystemMessagesWidget::instance()->writeMessage(
                    tr("The remote device was not found. Please check the "
                    "host address and port settings."),
                    SystemMessagesWidget::KError,
                    SystemMessagesWidget::KDialogAndTextbox);
        break;
    case QAbstractSocket::ConnectionRefusedError:
        SystemMessagesWidget::instance()->writeMessage(
                    tr("The connection was refused by the peer. "
                    "Make sure the application on the remote device is running, "
                    "and check that the host address and port "
                    "settings are correct."),
                    SystemMessagesWidget::KError,
                    SystemMessagesWidget::KDialogAndTextbox);
        break;
    default:
        SystemMessagesWidget::instance()->writeMessage(
                    QString(tr("Connection error: %1.")).arg(socketError),
                    SystemMessagesWidget::KError,
                    SystemMessagesWidget::KDialogAndTextbox);
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

    SystemMessagesWidget::instance()->writeMessage(tr("New connection arrived."),
                                                   SystemMessagesWidget::KDebug,
                                                   SystemMessagesWidget::KOnlyLogfile);

    timeout->start();

}

void NetworkCommunication::tcpDisconnected()
{
    QTcpSocket *socket = (QTcpSocket *) QObject::sender();

    QString ip = socket->peerAddress().toString();

    SystemMessagesWidget::instance()->writeMessage(tr("Disconnected from %1").arg(ip));

    if(m_tcpSocketMap->contains(ip)){
        // Cleanning the connections from the map.
        emit raspDisconnected(socket->peerAddress().toString());
        m_tcpSocketMap->remove(socket->peerAddress().toString());
    }else if(m_tcpSocket){
        //Disconnected from main current connection.
        m_tcpSocket = 0;
    }else{
        //Disconnected by timeout because the connection didnt the handshake.
        SystemMessagesWidget::instance()->writeMessage(
                    tr("The device of IP %1 didn't handshake, so the connection is now closed."),
                    SystemMessagesWidget::KDebug,
                    SystemMessagesWidget::KOnlyLogfile
                    );
    }

    socket->deleteLater();
}
