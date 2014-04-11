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
#include <QJsonObject>
#include <QFile>

#include <QJsonDocument>

#include <CoreLibrary/json/nodejsmessage.h>
#include <CoreLibrary/json/synchronizationpacket.h>

#include "rfidmonitordaemon.h"

RFIDMonitorDaemon::RFIDMonitorDaemon(QObject *parent) :
    QObject(parent),
    m_localServer(0),
    m_tcpSocket(0)
{
    m_localServer = new QLocalServer(0);
    m_tcpSocket = new QTcpSocket(0);

    m_serverName = "RFIDMonitorDaemon";

    connect(m_localServer, SIGNAL(newConnection()), SLOT(ipcNewConnection()));

    connect(m_tcpSocket, SIGNAL(connected()), SLOT(tcpConnected()));
    connect(m_tcpSocket, SIGNAL(disconnected()), SLOT(tcpDisconnected()));
    connect(m_tcpSocket, SIGNAL(readyRead()), SLOT(tcpReadyRead()));
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(tcpHandleError(QAbstractSocket::SocketError)));

    //    m_hostName = "179.106.217.11";
//    m_hostName = "179.106.217.28";
    m_hostName = "192.168.0.117";
//    m_hostName = "localhost";
    m_tcpPort = 8124;

    QString socketFile = QString("/tmp/%1").arg(m_serverName);

    if(QFile::exists(socketFile)){
        QString rmCommand = QString("rm -f %1").arg(socketFile);
        system(rmCommand.toStdString().c_str());
    }

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

    if(nodeMessage.type() == "data"){

#ifdef DEBUG_VERBOSE
        qDebug() << QString("RFIDMonitorDaemon -> IPC Message received: %1").arg(QString(data));
#endif
        m_tcpSocket->write(data);
        m_tcpSocket->flush();
    }else{
        QString message(data);

        //qDebug() << QString("RFIDMonitorDaemon -> IPC Message received: %1").arg(message);
        m_tcpSocket->write(QString("{\"type\": \"SYN-ERROR\", \"datetime\": \"%1\", \"data\": \"%2\"}").arg(QDateTime::currentDateTime().toString(Qt::ISODate)).arg(message).toLatin1());
        m_tcpSocket->flush();
    }
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
}

void RFIDMonitorDaemon::tcpDisconnected()
{
    QTimer::singleShot(5000, this, SLOT(tcpConnect()));
}

void RFIDMonitorDaemon::tcpReadyRead()
{
    QByteArray data = m_tcpSocket->readAll();

    QString message(data);
#ifdef DEBUG_VERBOSE
    qDebug() << QString("Message from Node.js: %1").arg(message);
#endif

    QJsonDocument jsonDoc = QJsonDocument::fromBinaryData(data);
    QJsonObject obj = jsonDoc.object();
    json::NodeJSMessage nodeMessage;
    nodeMessage.read(obj);

    if(nodeMessage.type() == "SYN"){
        int clientsCount = this->m_localServer->findChildren<QLocalSocket *>().size();

        json::NodeJSMessage answer;
        answer.setType("SYN-ERROR");
        answer.setDateTime(QDateTime::currentDateTime());
        answer.setJsonData("");
        QJsonObject jsonAnswer;
        answer.write(jsonAnswer);

        if(!clientsCount){
            m_tcpSocket->write(QJsonDocument(jsonAnswer).toJson());
            m_tcpSocket->flush();
        }else{
            //qDebug() << QString("");
        }
    }

    foreach (QLocalSocket *socket, this->m_localServer->findChildren<QLocalSocket *>()) {
        socket->write(data);
        socket->flush();
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

void RFIDMonitorDaemon::tcpSendMessage(const QString &message)
{
    foreach (QLocalSocket *socket, this->m_localServer->findChildren<QLocalSocket *>()) {
        socket->write(message.toLatin1());
    }
}
