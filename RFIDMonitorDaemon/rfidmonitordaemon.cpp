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

#ifdef DEBUG_LOGGER
    m_daemonLogger <<  "HostName: " << m_hostName;
    m_daemonLogger <<  "Port: " << m_tcpPort;
#endif
}

RFIDMonitorDaemon::~RFIDMonitorDaemon()
{
}

void RFIDMonitorDaemon::start()
{
    if(m_localServer->listen(m_serverName)){
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  QString("Server name: %1").arg(m_localServer->serverName());
#endif
        QProcess *rfidMonitor = new QProcess(this);
        rfidMonitor->start("./RFIDMonitor");
        connect(this, &RFIDMonitorDaemon::destroyed, rfidMonitor, &QProcess::kill);

        QThread *consoleThread = new QThread(this);
        Console *console = new Console;
        console->moveToThread(consoleThread);

        connect(consoleThread, SIGNAL(destroyed()), console, SLOT(deleteLater()));
        connect(consoleThread, &QThread::started, console, &Console::run);
        connect(console, &Console::exitApp, consoleThread, &QThread::quit);
        connect(console, SIGNAL(exitApp()), qApp, SLOT(quit()));

        //        consoleThread->start();

        QTimer::singleShot(100, this, SLOT(tcpConnect()));
    }else{
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "Could not listen!";
#endif
    }
}

void RFIDMonitorDaemon::ipcNewConnection()
{
#ifdef DEBUG_LOGGER
    m_daemonLogger <<  "New client connected.";
#endif
    QLocalSocket *clientConnection = m_localServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(readyRead()), SLOT(ipcReadyRead()));
    connect(clientConnection, &QLocalSocket::disconnected, [this]()
    {
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "Client Disconnected";
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

#ifdef DEBUG_LOGGER
    m_daemonLogger <<  QString("RFIDMonitorDaemon -> IPC Message received: %1").arg(QString(data));
#endif

    if(nodeMessage.type() == "SYN"){
        int clientsCount = this->m_localServer->findChildren<QLocalSocket *>().size();
        if(!clientsCount){
            m_tcpSocket->write(QString("{type: \"SYN\", datetime: \"%1\", data: \"\"}").arg(QDateTime::currentDateTime().toString(Qt::ISODate)).toLatin1());
        }else{
#ifdef DEBUG_LOGGER
            m_daemonLogger <<  QString("");
#endif
        }
    } else{
        m_tcpSocket->write(data);
        m_tcpSocket->flush();
    }
}

void RFIDMonitorDaemon::tcpConnect()
{    
    m_tcpSocket->connectToHost(m_hostName, m_tcpPort);
#ifdef DEBUG_LOGGER
    m_daemonLogger <<  QString("Establishing connection to %1 on port %2").arg(m_hostName).arg(m_tcpPort );
#endif
}

void RFIDMonitorDaemon::tcpConnected()
{
#ifdef DEBUG_LOGGER
    m_daemonLogger <<  QString("Connected to %1 on port %2").arg(m_hostName).arg(m_tcpPort );
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

    if( ! hasPackage){
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "waiting for package";
#endif
        if((quint64)m_tcpSocket->bytesAvailable() < sizeof(quint64))
            return;
        m_tcpSocket->read((char *)&packageSize, sizeof(quint64));
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "Size of comming package: " << packageSize;
#endif
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

#ifdef DEBUG_LOGGER
    m_daemonLogger <<  QString("Error: %1 - %2").arg(m_tcpSocket->error()).arg(m_tcpSocket->errorString());
#endif
}

void RFIDMonitorDaemon::tcpSendMessage(const QByteArray &message)
{
#ifdef DEBUG_LOGGER
    m_daemonLogger <<  "Message size: " << message.size();
#endif

    //PACKAGE SIZE
    QByteArray packageSize;
    QString packageSizeStr(QString::number(message.size()));
    packageSize.fill('0', sizeof(quint64) - packageSizeStr.size());
    packageSize.append(packageSizeStr);

    //    m_tcpSocket->write(packageSize);

    //    quint64 packageSize = message.size();
    //    QByteArray data;
    //    data.append((char*)&packageSize, sizeof(quint64));
    //    m_tcpSocket->write(data);

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

#ifdef DEBUG_LOGGER
    m_daemonLogger <<  QString("Message from Node.js: %1").arg(QString(message));
#endif

    if(messageType == "ACK-SYN"){
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "ACK-SYN Message";
#endif

        QJsonObject obj(nodeMessage.jsonData());

        m_configManager->setIdentification(obj);

        QJsonObject dataObj;
        QDateTime dateTime = QDateTime::fromString(nodeMessage.jsonDateTime(), Qt::ISODate);

        dataObj.insert("message", m_configManager->setDateTime(dateTime)? QString("Date/Time update successfully") : QString("Error to update Date/Time"));
        tcpSendMessage(buildMessage(dataObj, "ACK").toJson());

    }else if (messageType == "GET-CONFIG") {
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "GET-CONFIG Received";
#endif
        tcpSendMessage(buildMessage(m_configManager->currentConfig(), "CONFIG").toJson());

    }else if (messageType == "READER-COMMAND") {
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "READER-COMMAND Received";
#endif
        QJsonObject response(nodeMessage.jsonData());
        m_daemonLogger <<  "Command: " << response.value("command").toString();


    }else if (messageType == "READER-RESPONSE") {
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "READER-RESPONSE Received";
#endif
        QJsonObject response(nodeMessage.jsonData());
        tcpSendMessage(buildMessage(response, "READER-RESPONSE").toJson());

    }else if (messageType == "NEW-CONFIG") {
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "NEW-CONFIG Received";
#endif

        QJsonObject newConfig(nodeMessage.jsonData());
        QString message(m_configManager->newConfig(newConfig)? "New configuration saved successfully" : "Culd not save the new configuration");
        QJsonObject dataObj;
        dataObj.insert("message", message);
        tcpSendMessage(buildMessage(dataObj, "ACK-NEW-CONFIG").toJson());

    }else if (messageType == "BROADCAST") {
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "BROADCAST Received";
#endif




    }else if (messageType == "DATETIME") {
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "DATETIME Received";
#endif

        QJsonObject dataObj;
        QDateTime dateTime = QDateTime::fromString(nodeMessage.jsonDateTime(), Qt::ISODate);

        dataObj.insert("message", m_configManager->setDateTime(dateTime)? QString("Date/Time update successfully") : QString("Error to update Date/Time"));
        tcpSendMessage(buildMessage(dataObj, "ACK").toJson());

    }else if (messageType == "DATA") {
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "DATA Received";
#endif

    }else if (messageType == "ACK-DATA") {
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "ACK-DATA Received";
#endif

        QJsonObject ackData(nodeMessage.jsonData());
        QString hash = ackData["packageHash"].toString();
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "The package was successfulle synced. PACKAGE HASH: " << hash;
#endif
    }else if (messageType == "GET-NET-CONFIG") {
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "GET-NET-CONFIG Received";
#endif
        tcpSendMessage(buildMessage(m_configManager->netConfig(), "NET-CONFIG").toJson());

    }else if (messageType == "NEW-NET") {
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "NEW-NET Received";
#endif

        QJsonObject netConfig(nodeMessage.jsonData());
        QString message(m_configManager->setNetConfig(netConfig)? "New network configuration saved successfully" : "Culd not save the new network configuration");
        QJsonObject dataObj;
        dataObj.insert("message", message);
        tcpSendMessage(buildMessage(dataObj, "ACK-NET").toJson());
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  QString(m_configManager->restartNetwork()? "Network restarted" : "Networkt Don't restarted");
#endif
    }else if (messageType == "ACK-UNKNOWN") {
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "The server don't understand the message type: " << nodeMessage.jsonData().value("type").toString();
#endif
    }
    else{
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "UNKNOWN MESSAGE";
#endif
        tcpSendMessage(buildMessage(QJsonDocument::fromJson(message).object(), "ACK-UNKNOWN").toJson());
    }
}
