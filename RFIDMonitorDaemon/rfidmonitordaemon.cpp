/****************************************************************************
**
** WWW.FISHMONITORING.COM.BR
**
** Copyright (C) 2013
**                     Luis Valdes <luisvaldes88@gmail.com>
**
** This file is part of the FishMonitoring project
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

#include <QLocalServer>
#include <QLocalSocket>
#include <QTcpSocket>
#include <QDebug>
#include <QDateTime>

#include <QNetworkInterface>
#include <QFile>

#include "CoreLibrary/json/nodejsmessage.h"
#include "CoreLibrary/json/rfidmonitorsettings.h"


#include "rfidmonitordaemon.h"

RFIDMonitorDaemon::RFIDMonitorDaemon(QObject *parent) :
    QObject(parent),
    m_localServer(0),
    m_tcpSocket(0),
    m_tcpAppSocket(0)
{
    m_localServer = new QLocalServer(this);
    m_tcpSocket = new QTcpSocket(this);
    m_udpSocket = new QUdpSocket(this);
    m_tcpAppSocket = new QTcpSocket(this);

    m_serverName = "RFIDMonitorDaemon";

    m_configManager = new ConfigManager(this);

    connect(m_localServer, SIGNAL(newConnection()), SLOT(ipcNewConnection()));

    connect(m_tcpSocket, SIGNAL(connected()), SLOT(tcpConnected()));
    connect(m_tcpSocket, SIGNAL(disconnected()), SLOT(tcpDisconnected()));
    connect(m_tcpSocket, SIGNAL(readyRead()), SLOT(routeMessageTcp()));
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(tcpHandleError(QAbstractSocket::SocketError)));

    connect(m_udpSocket, SIGNAL(readyRead()), SLOT(readDatagrams()));

    connect(m_tcpAppSocket, &QTcpSocket::connected, ([=] () { m_udpSocket->close(); qDebug() << "Connected with DeskApp";}));
    connect(m_tcpAppSocket, &QTcpSocket::disconnected,
            ([=] () {
            qDebug() << "Connection Closed";
            if(!m_udpSocket->bind(QHostAddress::Any, 9999)){
                qDebug() << QString("Couldn't listening broadcast");
             };
            }));

    connect(m_tcpAppSocket, SIGNAL(readyRead()), SLOT(routeMessageTcp()));

    QString socketFile = QString("/tmp/%1").arg(m_serverName);

    if(QFile::exists(socketFile)){
        QString rmCommand = QString("rm -f %1").arg(socketFile);
        system(rmCommand.toStdString().c_str());
    }
    m_hostName = m_configManager->hostName();
    m_tcpPort = m_configManager->hostPort();

    qDebug() << "HostName: " << m_hostName;
    qDebug() << "Port: " << m_tcpPort;
}

RFIDMonitorDaemon::~RFIDMonitorDaemon()
{
}

void RFIDMonitorDaemon::start()
{
    if(m_localServer->listen(m_serverName)){
#ifdef DEBUG_VERBOSE
        qDebug() << QString("Server name: %1").arg(m_localServer->serverName());
#endif

        //        QThread *consoleThread = new QThread(this);
        //        Console *console = new Console;
        //        console->moveToThread(consoleThread);

        //        connect(consoleThread, SIGNAL(destroyed()), console, SLOT(deleteLater()));
        //        connect(consoleThread, &QThread::started, console, &Console::run);
        //        connect(console, SIGNAL(sendMessage(QByteArray)), this, SLOT(tcpSendMessage(QByteArray)));
        //        connect(console, &Console::exitApp, consoleThread, &QThread::quit);
        //        connect(console, SIGNAL(exitApp()), qApp, SLOT(quit()));

        //        consoleThread->start();

        QTimer::singleShot(100, this, SLOT(tcpConnect()));
    }else{
#ifdef DEBUG_VERBOSE
        qDebug() << "Could not listen!";
#endif
    }
    if(!m_udpSocket->bind(QHostAddress::Any, 9999))
        qDebug() << QString("Couldn't listening broadcast");
}

void RFIDMonitorDaemon::ipcNewConnection()
{
#ifdef DEBUG_VERBOSE
    qDebug() << "New client connected.";
#endif
    //    QLocalSocket *clientConnection = m_localServer->nextPendingConnection();
    ipcConnection = m_localServer->nextPendingConnection();
    connect(ipcConnection, SIGNAL(readyRead()), SLOT(ipcReadyRead()));
    connect(ipcConnection, &QLocalSocket::disconnected, []()
    {
#ifdef DEBUG_VERBOSE
        qDebug() << "Client Disconnected";
#endif
    });

    connect(ipcConnection, SIGNAL(disconnected()), ipcConnection, SLOT(deleteLater()));
    //    ipcConnection->write("Welcome to IPC server\n");
}

void RFIDMonitorDaemon::ipcReadyRead()
{


    //    ipcConnection = qobject_cast<QLocalSocket *>(sender());
    //    routeMessageIpc(ipcConnection->readAll());
}

void RFIDMonitorDaemon::tcpConnect()
{
    m_tcpSocket->connectToHost(m_hostName, m_tcpPort);
#ifdef DEBUG_VERBOSE
    qDebug() << QString("Establishing connection to %1 on port %2").arg(m_hostName).arg(m_tcpPort );
#endif
}

void RFIDMonitorDaemon::tcpConnected()
{
#ifdef DEBUG_VERBOSE
    qDebug() << QString("Connected to %1 on port %2").arg(m_hostName).arg(m_tcpPort );
#endif

    tcpSendMessage((QTcpSocket *) QObject::sender(), buildMessage(m_configManager->identification(), "SYN").toJson());
}

void RFIDMonitorDaemon::tcpDisconnected()
{
    QTimer::singleShot(5000, this, SLOT(tcpConnect()));
}

void RFIDMonitorDaemon::tcpReadyRead()
{
    //    QTcpSocket *connection = (QTcpSocket *) QObject::sender();

    //    static bool hasPackage = false;
    //    static quint64 packageSize = 0;

    //    if( ! hasPackage){
    //        qDebug() << "waiting for package";

    //        if((quint64)connection->bytesAvailable() < sizeof(quint64))
    //            return;

    //        //        m_tcpSocket->read((char *)&packageSize, sizeof(quint64));

    //        QString packageSizeStr(connection->read(sizeof(quint64)));
    //        packageSize = packageSizeStr.toULongLong();

    //        qDebug() << "Size of comming package: " << packageSize;
    //        hasPackage = true;
    //    }

    //    if((quint64)connection->bytesAvailable() >=  packageSize){
    //        QByteArray data(connection->read(packageSize));
    //        packageSize = 0;
    //        hasPackage = false;
    //        routeMessageTcp(data);
    //    } else {
    //        qDebug() << "no bytes available";
    //    }
}

void RFIDMonitorDaemon::tcpHandleError(QAbstractSocket::SocketError error )
{
    if(error == QAbstractSocket::ConnectionRefusedError)
        tcpDisconnected();

#ifdef DEBUG_VERBOSE
    qDebug() << QString("Error: %1 - %2").arg(m_tcpSocket->error()).arg(m_tcpSocket->errorString());
#endif
}

void RFIDMonitorDaemon::tcpSendMessage(QTcpSocket *con, const QByteArray &message)
{
    qDebug() << "Message size: " << message.size();

    if(con->isOpen()){
        //PACKAGE SIZE
        QByteArray packageSize;
        QString packageSizeStr(QString::number(message.size()));
        packageSize.fill('0', sizeof(quint64) - packageSizeStr.size());
        packageSize.append(packageSizeStr);

        con->write(packageSize);

        //    quint64 packageSize = message.size();
        //    QByteArray dataSize;
        //    dataSize.append((char*)&packageSize, sizeof(quint64));
        //    m_tcpSocket->write(dataSize);

        con->write(message);
    } else {
        qDebug() << "Doesn't work mada faka";
    }

    //    if(m_tcpSocket->isOpen()){
    //        //PACKAGE SIZE
    //        QByteArray packageSize;
    //        QString packageSizeStr(QString::number(message.size()));
    //        packageSize.fill('0', sizeof(quint64) - packageSizeStr.size());
    //        packageSize.append(packageSizeStr);

    //        m_tcpSocket->write(packageSize);

    //        //    quint64 packageSize = message.size();
    //        //    QByteArray dataSize;
    //        //    dataSize.append((char*)&packageSize, sizeof(quint64));
    //        //    m_tcpSocket->write(dataSize);

    //        m_tcpSocket->write(message);
    //    } else {
    //        QTimer::singleShot(100, this, SLOT(tcpConnect()));
    //    }
}

void RFIDMonitorDaemon::ipcSendMessage(const QByteArray &message)
{
    if(ipcConnection)
        ipcConnection->write(message);
}

QJsonDocument RFIDMonitorDaemon::buildMessage(QJsonObject dataObj, QString type)
{
    QJsonObject rootObj;
    QJsonDocument rootDoc;

    rootObj.insert("type", type);
    rootObj.insert("datetime", QJsonValue(QDateTime::currentDateTime().toString(Qt::ISODate)));
    rootObj.insert("data", QJsonValue(dataObj));

    rootDoc.setObject(rootObj);
    return rootDoc;
}

void RFIDMonitorDaemon::routeMessageTcp()
{
    QTcpSocket *connection = (QTcpSocket *) QObject::sender();

    static bool hasPackage = false;
    static quint64 packageSize = 0;

    if( ! hasPackage){
        qDebug() << "waiting for package";

        if((quint64)connection->bytesAvailable() < sizeof(quint64))
            return;

        //        m_tcpSocket->read((char *)&packageSize, sizeof(quint64));

        QString packageSizeStr(connection->read(sizeof(quint64)));
        packageSize = packageSizeStr.toULongLong();

        qDebug() << "Size of comming package: " << packageSize;
        hasPackage = true;
    }

    if((quint64)connection->bytesAvailable() >=  packageSize){
        QByteArray data(connection->read(packageSize));
        packageSize = 0;
        hasPackage = false;

        json::NodeJSMessage nodeMessage;

        nodeMessage.read(QJsonDocument::fromJson(data).object());
        QString messageType(nodeMessage.type());

        if(messageType == "ACK-SYN"){
            qDebug() << "ACK-SYN Message";

            qDebug() << QString(QJsonDocument(nodeMessage.jsonData()).toJson());

            //        QJsonObject obj(nodeMessage.jsonData());

            //        m_configManager->setIdentification(obj);

            QJsonObject dataObj;
            //        QDateTime dateTime = nodeMessage.dateTime();
            //        QDateTime dateTime = QDateTime::fromString(nodeMessage.jsonDateTime(), Qt::ISODate);

            //        dataObj.insert("message", m_configManager->setDateTime(dateTime)? QString("Date/Time update successfully") : QString("Error to update Date/Time"));
            tcpSendMessage(connection, buildMessage(dataObj, "ACK").toJson());

        }else if (messageType == "GET-CONFIG") {
            qDebug() << "GET-CONFIG Received";
            tcpSendMessage(connection, buildMessage(m_configManager->currentConfig(), "CONFIG").toJson());

        }else if (messageType == "READER-COMMAND") {

            qDebug() << "READER-COMMAND Received";

            QJsonObject response(nodeMessage.jsonData());
            qDebug() << "Command: " << response.value("command").toString();


        }else if (messageType == "NEW-CONFIG") {
            qDebug() << "NEW-CONFIG Received";

            QJsonObject newConfig(nodeMessage.jsonData());
            QString message(m_configManager->newConfig(newConfig)? "New configuration saved successfully" : "Culd not save the new configuration");
            QJsonObject dataObj;
            dataObj.insert("message", message);
            tcpSendMessage(connection, buildMessage(dataObj, "ACK-NEW-CONFIG").toJson());
        }
        else if (messageType == "DATETIME") {
            qDebug() << "DATETIME Received";

            QJsonObject dataObj;
            QDateTime dateTime = nodeMessage.dateTime();
            //        QDateTime dateTime = QDateTime::fromString(nodeMessage.jsonDateTime(), Qt::ISODate);

            dataObj.insert("message", m_configManager->setDateTime(dateTime)? QString("Date/Time update successfully") : QString("Error to update Date/Time"));
            tcpSendMessage(connection, buildMessage(dataObj, "ACK").toJson());

        }else if (messageType == "ACK-DATA") {
            qDebug() << "ACK-DATA Received";

            ipcSendMessage(data);

            QJsonObject ackData(nodeMessage.jsonData());
            QString hash = ackData["md5diggest"].toString();
            qDebug() << "The package was successfulle synced. PACKAGE HASH: " << hash;

        }else if (messageType == "GET-NET-CONFIG") {
            qDebug() << "GET-NET-CONFIG Received";
            tcpSendMessage(connection, buildMessage(m_configManager->netConfig(), "NET-CONFIG").toJson());

        }else if (messageType == "NEW-NET") {
            qDebug() << "NEW-NET Received";

            QJsonObject netConfig(nodeMessage.jsonData());
            QString message(m_configManager->setNetConfig(netConfig)? "New network configuration saved successfully" : "Culd not save the new network configuration");
            QJsonObject dataObj;
            dataObj.insert("message", message);
            tcpSendMessage(connection, buildMessage(dataObj, "ACK-NET").toJson());

            qDebug() << QString(m_configManager->restartNetwork()? "Network restarted" : "Networkt Don't restarted");

        }else if (messageType == "ACK-UNKNOWN") {
            qDebug() << "The server don't understand the message type: " << nodeMessage.jsonData().value("type").toString();
        }
        else{
            qDebug() << "UNKNOWN MESSAGE";
            tcpSendMessage(connection, buildMessage(QJsonDocument::fromJson(data).object(), "ACK-UNKNOWN").toJson());
        }
    } else {
        qDebug() << "no bytes available";
    }
}

void RFIDMonitorDaemon::routeMessageIpc()
{
    QByteArray message = ipcConnection->readAll();
    json::NodeJSMessage nodeMessage;

    nodeMessage.read(QJsonDocument::fromJson(message).object());
    QString messageType(nodeMessage.type());

    if(messageType == "SYN"){
        qDebug() << "SYN Message received from MONITOR";
        ipcSendMessage(buildMessage(QJsonObject(), "ACK-SYN").toJson());

    }else if (messageType == "READER-RESPONSE") {

        qDebug() << "READER-RESPONSE Received";
        tcpSendMessage(m_tcpSocket, message);//buildMessage(QJsonObject(nodeMessage.jsonData()), "READER-RESPONSE").toJson());

    }else if (messageType == "DATA") {
        qDebug() << "DATA Received\n";
        qDebug() << QString(QJsonDocument(nodeMessage.jsonData()).toJson());
        tcpSendMessage(m_tcpSocket, message);//buildMessage(nodeMessage.jsonData(), "DATA").toJson());

    }else if (messageType == "ACK-UNKNOWN") {
        qDebug() << "The MONITOR don't understand the message type: " << nodeMessage.jsonData().value("type").toString();
    }
    else{
        qDebug() << "UNKNOWN MESSAGE";
        ipcSendMessage(buildMessage(QJsonDocument::fromJson(message).object(), "ACK-UNKNOWN").toJson());
    }
}

void RFIDMonitorDaemon::readDatagrams()
{
    qDebug() << "New Datagram arrived";
    QByteArray datagram;
    datagram.resize(m_udpSocket->pendingDatagramSize());
    QHostAddress sender;

    m_udpSocket->readDatagram(datagram.data(), datagram.size(), &sender);

    QJsonDocument broacast(QJsonDocument::fromJson(datagram));
    m_tcpAppSocket->connectToHost(sender, broacast.object().value("port").toInt());
    tcpSendMessage(m_tcpAppSocket, buildMessage(m_configManager->identification(), "SYN").toJson());
}
