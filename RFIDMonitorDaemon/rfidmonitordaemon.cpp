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
    m_tcpSocket(0)
{
    m_localServer = new QLocalServer(this);
    m_tcpSocket = new QTcpSocket(this);

    m_serverName = "RFIDMonitorDaemon";

    m_configManager = new ConfigManager(this);
    connect(m_configManager, SIGNAL(sendTcpMessage(QByteArray)), SLOT(tcpSendMessage(QByteArray)));

    connect(m_localServer, SIGNAL(newConnection()), SLOT(ipcNewConnection()));

    connect(m_tcpSocket, SIGNAL(connected()), SLOT(tcpConnected()));
    connect(m_tcpSocket, SIGNAL(disconnected()), SLOT(tcpDisconnected()));
    connect(m_tcpSocket, SIGNAL(readyRead()), SLOT(tcpReadyRead()));
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(tcpHandleError(QAbstractSocket::SocketError)));

    QString socketFile = QString("/tmp/%1").arg(m_serverName);

    if(QFile::exists(socketFile)){
        QString rmCommand = QString("rm -f %1").arg(socketFile);
        system(rmCommand.toStdString().c_str());
    }
    m_hostName = m_configManager->hostName();
    m_tcpPort = m_configManager->hostPort();

    qDebug() << "HostName: " << m_hostName;
    qDebug() << "Port: " << m_tcpPort;

    //m_hostName = "179.106.217.27";
    //m_hostName = "179.106.217.11";
    //m_tcpPort = 8124;
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

        QThread *consoleThread = new QThread(this);
        Console *console = new Console;
        console->moveToThread(consoleThread);

        connect(consoleThread, SIGNAL(destroyed()), console, SLOT(deleteLater()));
        connect(consoleThread, &QThread::started, console, &Console::run);
        connect(console, SIGNAL(sendMessage(QString)), this, SLOT(tcpSendMessage(QString)));
        connect(console, &Console::exitApp, consoleThread, &QThread::quit);
        connect(console, SIGNAL(exitApp()), qApp, SLOT(quit()));

//        consoleThread->start();

        QTimer::singleShot(100, this, SLOT(tcpConnect()));
    }else{
#ifdef DEBUG_VERBOSE
        qDebug() << "Could not listen!";
#endif
    }
}

void RFIDMonitorDaemon::ipcNewConnection()
{
#ifdef DEBUG_VERBOSE
    qDebug() << "New client connected.";
#endif
    QLocalSocket *clientConnection = m_localServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(readyRead()), SLOT(ipcReadyRead()));
    connect(clientConnection, &QLocalSocket::disconnected, []()
    {
#ifdef DEBUG_VERBOSE
        qDebug() << "Client Disconnected";
#endif
    });

    connect(clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));
    clientConnection->write("Welcome to IPC server\n");
}

void RFIDMonitorDaemon::ipcReadyRead()
{
    QLocalSocket *clientConnection = qobject_cast<QLocalSocket *>(sender());

    QByteArray data = clientConnection->readAll();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    QJsonObject obj = jsonDoc.object();
    json::NodeJSMessage nodeMessage;
    nodeMessage.read(obj);

#ifdef DEBUG_VERBOSE
        qDebug() << QString("RFIDMonitorDaemon -> IPC Message received: %1").arg(QString(data));
#endif

    if(nodeMessage.type() == "SYN"){
        int clientsCount = this->m_localServer->findChildren<QLocalSocket *>().size();
        if(!clientsCount){
            m_tcpSocket->write(QString("{type: \"SYN\", datetime: \"%1\", data: \"\"}").arg(QDateTime::currentDateTime().toString(Qt::ISODate)).toLatin1());
        }else{
            qDebug() << QString("");
        }
    }
    else{
        QString message(data);

        //qDebug() << QString("RFIDMonitorDaemon -> IPC Message received: %1").arg(message);
        m_tcpSocket->write(QString("{\"type\": \"SYN-ERROR\", \"datetime\": \"%1\", \"data\": \"%2\"}").arg(QDateTime::currentDateTime().toString(Qt::ISODate)).arg(message).toLatin1());
        m_tcpSocket->flush();
    }
//        m_tcpSocket->write(data);
//        m_tcpSocket->flush();
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

    tcpSendMessage(buildMessage(m_configManager->identification(), "SYN").toJson());
}

void RFIDMonitorDaemon::tcpDisconnected()
{
    QTimer::singleShot(5000, this, SLOT(tcpConnect()));
}

void RFIDMonitorDaemon::tcpReadyRead()
{
    static bool hasPackage = false;
    static quint64 packageSize = 0;

#ifdef DEBUG_VERBOSE
    qDebug() << QString("Message from Node.js: %1").arg(message);
#endif
    if( ! hasPackage){
        qDebug() << "waiting for package";

        if((quint64)m_tcpSocket->bytesAvailable() < sizeof(quint64))
            return;
        m_tcpSocket->read((char *)&packageSize, sizeof(quint64));

        qDebug() << "Size of comming package: " << packageSize;
        hasPackage = true;
    }

    if((quint64)m_tcpSocket->bytesAvailable() >=  packageSize){
        QByteArray data(m_tcpSocket->read(packageSize));
        routeMessage(data);
    }
}

void RFIDMonitorDaemon::tcpHandleError(QAbstractSocket::SocketError error )
{
    if(error == QAbstractSocket::ConnectionRefusedError)
        tcpDisconnected();

#ifdef DEBUG_VERBOSE
    qDebug() << QString("Error: %1 - %2").arg(m_tcpSocket->error()).arg(m_tcpSocket->errorString());
#endif
}

void RFIDMonitorDaemon::tcpSendMessage(const QByteArray &message)
{
    qDebug() << "Message size: " << message.size();

    //PACKAGE SIZE
    //    QByteArray packageSize;
    //    QString packageSizeStr(QString::number(message.size()));
    //    packageSize.fill('0', sizeof(quint64) - packageSizeStr.size());
    //    packageSize.append(packageSizeStr);

    //    m_tcpSocket->write(packageSize);

    quint64 packageSize = message.size();
    QByteArray data;
    data.append((char*)&packageSize, sizeof(quint64));

    m_tcpSocket->write(data);
    m_tcpSocket->write(message);
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

void RFIDMonitorDaemon::routeMessage(const QByteArray &message)
{
    json::NodeJSMessage nodeMessage;

    nodeMessage.read(QJsonDocument::fromJson(message).object());
    QString messageType(nodeMessage.type());

    if(messageType == "ACK-SYN"){
        qDebug() << "ACK-SYN Message";

        QJsonObject obj(nodeMessage.jsonData());

        m_configManager->setIdentification(obj);

        QJsonObject dataObj;
        QDateTime dateTime = QDateTime::fromString(nodeMessage.jsonDateTime(), Qt::ISODate);

        dataObj.insert("message", m_configManager->setDateTime(dateTime)? QString("Date/Time update successfully") : QString("Error to update Date/Time"));
        tcpSendMessage(buildMessage(dataObj, "ACK").toJson());

    }else if (messageType == "GET-CONFIG") {
        qDebug() << "GET-CONFIG Received";
        tcpSendMessage(buildMessage(m_configManager->currentConfig(), "CONFIG").toJson());

    }else if (messageType == "READER-COMMAND") {

        qDebug() << "READER-COMMAND Received";

        QJsonObject response(nodeMessage.jsonData());
        qDebug() << "Command: " << response.value("command").toString();


    }else if (messageType == "READER-RESPONSE") {

        qDebug() << "READER-RESPONSE Received";
        QJsonObject response(nodeMessage.jsonData());
        tcpSendMessage(buildMessage(response, "READER-RESPONSE").toJson());

    }else if (messageType == "NEW-CONFIG") {
        qDebug() << "NEW-CONFIG Received";

        QJsonObject newConfig(nodeMessage.jsonData());
        QString message(m_configManager->newConfig(newConfig)? "New configuration saved successfully" : "Culd not save the new configuration");
        QJsonObject dataObj;
        dataObj.insert("message", message);
        tcpSendMessage(buildMessage(dataObj, "ACK-NEW-CONFIG").toJson());

    }else if (messageType == "BROADCAST") {
        qDebug() << "BROADCAST Received";




    }else if (messageType == "DATETIME") {
        qDebug() << "DATETIME Received";

        QJsonObject dataObj;
        QDateTime dateTime = QDateTime::fromString(nodeMessage.jsonDateTime(), Qt::ISODate);

        dataObj.insert("message", m_configManager->setDateTime(dateTime)? QString("Date/Time update successfully") : QString("Error to update Date/Time"));
        tcpSendMessage(buildMessage(dataObj, "ACK").toJson());

    }else if (messageType == "DATA") {
        qDebug() << "DATA Received";

    }else if (messageType == "ACK-DATA") {
        qDebug() << "ACK-DATA Received";

        QJsonObject ackData(nodeMessage.jsonData());
        QString hash = ackData["packageHash"].toString();
        qDebug() << "The package was successfulle synced. PACKAGE HASH: " << hash;

    }else if (messageType == "GET-NET-CONFIG") {
        qDebug() << "GET-NET-CONFIG Received";
        tcpSendMessage(buildMessage(m_configManager->netConfig(), "NET-CONFIG").toJson());

    }else if (messageType == "NEW-NET") {
        qDebug() << "NEW-NET Received";

        QJsonObject netConfig(nodeMessage.jsonData());
        QString message(m_configManager->setNetConfig(netConfig)? "New network configuration saved successfully" : "Culd not save the new network configuration");
        QJsonObject dataObj;
        dataObj.insert("message", message);
        tcpSendMessage(buildMessage(dataObj, "ACK-NET").toJson());

        qDebug() << QString(m_configManager->restartNetwork()? "Network restarted" : "Networkt Don't restarted");

    }else if (messageType == "ACK-UNKNOWN") {
        qDebug() << "The server don't understand the message type: " << nodeMessage.jsonData().value("type").toString();
    }
    else{
        qDebug() << "UNKNOWN MESSAGE";
        tcpSendMessage(buildMessage(QJsonDocument::fromJson(message).object(), "ACK-UNKNOWN").toJson());
    }
}
