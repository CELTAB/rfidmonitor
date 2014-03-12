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

#ifndef RFIDMONITORDAEMON_H
#define RFIDMONITORDAEMON_H

#include <QObject>
#include <QAbstractSocket>

class QLocalServer;
class QTcpSocket;
class QLocalSocket;

class RFIDMonitorDaemon : public QObject
{
    Q_OBJECT
public:
    explicit RFIDMonitorDaemon(QObject *parent = 0);

    void start();

public slots:
    void ipcNewConnection();
    void ipcReadyRead();

    void tcpConnect();
    void tcpConnected();
    void tcpDisconnected();
    void tcpReadyRead();
    void tcpHandleError(QAbstractSocket::SocketError);

private:
    QLocalServer *m_localServer;
    QTcpSocket *m_tcpSocket;
    QString m_serverName;
};

#endif // RFIDMONITORDAEMON_H
