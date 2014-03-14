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

#include <QLocalSocket>
#include <QTcpSocket>

#include "communicationservice.h"

CommunicationService::CommunicationService(QObject *parent) :
    CommunicationInterface(parent),
    m_localSocket(0)
{
    m_localSocket = new QLocalSocket(this);

    connect(m_localSocket, SIGNAL(connected()), SLOT(ipcConnected()));
    connect(m_localSocket, SIGNAL(disconnected()), SLOT(ipcDisconnected()));
    connect(m_localSocket, SIGNAL(readyRead()), SLOT(ipcReadyRead()));
    connect(m_localSocket, SIGNAL(error(QLocalSocket::LocalSocketError)), SLOT(ipcHandleError(QLocalSocket::LocalSocketError)));

    m_localSocket->connectToServer("RFIDMonitorDaemon");
}

QString CommunicationService::serviceName() const
{
    return "communication.service";
}

void CommunicationService::init()
{
}

ServiceType CommunicationService::type()
{
    return ServiceType::KCommunicationService;
}

void CommunicationService::sendMessage(QByteArray value)
{
    m_localSocket->write(value);
}

void CommunicationService::ipcConnected()
{
    qDebug() << QString("CommunicationService -> Connected successfully to IPC Server.");
    m_localSocket->write(QString("System Started").toLatin1());
}

void CommunicationService::ipcDisconnected()
{
    qDebug() << QString("CommunicationService -> Disconected from IPC Server.");
}

void CommunicationService::ipcReadyRead()
{
    QByteArray data = m_localSocket->readAll();
    QString message(data);
    qDebug() << QString("CommunicationService -> Message received: %1").arg(message);
    emit messageReceived(data);
}

void CommunicationService::ipcHandleError(QLocalSocket::LocalSocketError)
{
    qDebug() << QString("Error: %1 - %2").arg(m_localSocket->error()).arg(m_localSocket->errorString());
}
