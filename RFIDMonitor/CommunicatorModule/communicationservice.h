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

#ifndef COMMUNICATIONSERVICE_H
#define COMMUNICATIONSERVICE_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>

#include <QLocalSocket>
#include <core/interfaces.h>

class CommunicationService : public CommunicationInterface
{
    Q_OBJECT
public:
    explicit CommunicationService(QObject *parent = 0);

    QString serviceName() const;
    void init();
    ServiceType type();

    void sendMessage(QJsonObject data, QString type);
    void sendMessage(QByteArray value);

public slots:
    void ipcConnected();
    void ipcDisconnected();
    void ipcReadyRead();
    void ipcHandleError(QLocalSocket::LocalSocketError);

private:
    QLocalSocket *m_localSocket;
};

#endif // COMMUNICATIONSERVICE_H
