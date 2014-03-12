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
#include <QTimer>
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

}

void RFIDMonitorDaemon::start()
{
   m_localServer->listen(m_serverName);
   qDebug() << QString("Server name: %1").arg(m_localServer->serverName());
}

void RFIDMonitorDaemon::ipcNewConnection()
{
    QLocalSocket *clientConnection = m_localServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(readyRead()), SLOT(ipcReadyRead()));

    connect(clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));
}

void RFIDMonitorDaemon::ipcReadyRead()
{
    QLocalSocket *clientConnection = qobject_cast<QLocalSocket *>(sender());

    QDataStream in(clientConnection);
    in.setVersion(QDataStream::Qt_5_2);
    if (clientConnection->bytesAvailable() < (int)sizeof(quint16)) {
        return;
    }
    QString message;
    in >> message;

    qDebug() << QString("RFIDMonitorDaemon -> Message received: %1").arg(message);

    m_tcpSocket->write(message.toLatin1());
}

void RFIDMonitorDaemon::tcpConnect()
{
    m_tcpSocket->connectToHost("localhost", 3000);
}

void RFIDMonitorDaemon::tcpConnected()
{
    qDebug() << QString("Connected to %1 on port ").arg("localhost").append(3000);
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
