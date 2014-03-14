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

    m_hostName = "179.106.217.11";
    m_tcpPort = 8124;

}

RFIDMonitorDaemon::~RFIDMonitorDaemon()
{
    system(QString("rm -rf /tmp/%1").arg(m_localServer->serverName()).toStdString().c_str());
}

void RFIDMonitorDaemon::start()
{

   if(m_localServer->listen(m_serverName)){
    qDebug() << QString("Server name: %1").arg(m_localServer->serverName());

    QThread *consoleThread = new QThread(this);
    Console *console = new Console;
    console->moveToThread(consoleThread);

    connect(consoleThread, SIGNAL(destroyed()), console, SLOT(deleteLater()));
    connect(consoleThread, &QThread::started, console, &Console::run);
    connect(console, SIGNAL(sendMessage(QString)), this, SLOT(tcpSendMessage(QString)));
    connect(console, &Console::exitApp, consoleThread, &QThread::quit);
    connect(console, SIGNAL(exitApp()), qApp, SLOT(quit()));

//    consoleThread->start();

    QTimer::singleShot(100, this, SLOT(tcpConnect()));
   }else{
    qDebug() << "Could not listen!";
   }
}

void RFIDMonitorDaemon::ipcNewConnection()
{
    qDebug() << "New client connected.";
    QLocalSocket *clientConnection = m_localServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(readyRead()), SLOT(ipcReadyRead()));
    connect(clientConnection, &QLocalSocket::disconnected, [](){ qDebug() << "Client Disconnected";});

    connect(clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));
    clientConnection->write("Welcome to IPC server\n");
}

void RFIDMonitorDaemon::ipcReadyRead()
{
    QLocalSocket *clientConnection = qobject_cast<QLocalSocket *>(sender());

    QByteArray data = clientConnection->readAll();
    QString message(data);

    qDebug() << QString("RFIDMonitorDaemon -> Message received: %1").arg(message);

    m_tcpSocket->write(data);
}

void RFIDMonitorDaemon::tcpConnect()
{    
    m_tcpSocket->connectToHost(m_hostName, m_tcpPort);
}

void RFIDMonitorDaemon::tcpConnected()
{
    qDebug() << QString("Connected to %1 on port %2").arg("localhost").arg(m_tcpPort);
}

void RFIDMonitorDaemon::tcpDisconnected()
{
    QTimer::singleShot(5000, this, SLOT(tcpConnect()));
}

void RFIDMonitorDaemon::tcpReadyRead()
{
    QByteArray data = m_tcpSocket->readAll();

    QString message(data);
    qDebug() << QString("Message from Node.js: %1").arg(message);
}

void RFIDMonitorDaemon::tcpHandleError(QAbstractSocket::SocketError)
{
    qDebug() << QString("Error: %1 - %2").arg(m_tcpSocket->error()).arg(m_tcpSocket->errorString());
}

void RFIDMonitorDaemon::tcpSendMessage(const QString &message)
{
    foreach (QLocalSocket *socket, this->m_localServer->findChildren<QLocalSocket *>()) {
        socket->write(message.toLatin1());
    }
}
