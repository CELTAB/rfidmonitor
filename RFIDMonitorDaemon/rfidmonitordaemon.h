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

#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QUdpSocket>

#include "configmanager.h"

class QLocalServer;
class QTcpSocket;
class QLocalSocket;

class DaemonLogger
{
public:
    explicit DaemonLogger()
    {
        m_daemonLogs.setFileName("daemon_debug.log");
        if (!m_daemonLogs.open(QIODevice::WriteOnly | QIODevice::Text)){

        }
    }

    DaemonLogger &operator<<(int message)
    {
        #ifdef DEBUG_VERBOSE
            qDebug() << message;
        #else
            QTextStream out(&m_daemonLogs);
            QString record("99999 % ");
            QString currDate = QDateTime::currentDateTime().toString("MM-dd-yyyy_hh:mm:ss");
            record.append(currDate + " % " + " % " + QString::number(message) + "\n");
            out << record;
        #endif
        return *this;
    }

    DaemonLogger &operator<<(const QString & message)
    {
        #ifdef DEBUG_VERBOSE
            qDebug() << message;
        #else
            QTextStream out(&m_daemonLogs);
            QString record("99999 % ");
            QString currDate = QDateTime::currentDateTime().toString("MM-dd-yyyy_hh:mm:ss");
            record.append(currDate + " % " + " % " + message + "\n");
            out << record;
        #endif
        return *this;
    }

private:
    QFile m_daemonLogs;
};

class RFIDMonitorDaemon : public QObject
{
    Q_OBJECT
public:
    explicit RFIDMonitorDaemon(QObject *parent = 0);
    ~RFIDMonitorDaemon();

    void start();

public slots:
    void ipcNewConnection();
    void ipcSendMessage(const QByteArray &message);

    void tcpConnect();
    void tcpConnected();
    void tcpDisconnected();
    void tcpHandleError(QAbstractSocket::SocketError error);

    void tcpSendMessage(QTcpSocket *con, const QByteArray &message);

    void readDatagrams();
    void routeMessageTcp();
    void routeMessageIpc();

private:
    QLocalServer *m_localServer;
    QLocalSocket *ipcConnection;

    // Node.js Server
    QTcpSocket *m_tcpSocket;

    // Desktop Application Only
    QTcpSocket *m_tcpAppSocket;
    QUdpSocket *m_udpSocket;

    QString m_serverName;
    QString m_hostName;
    int m_tcpPort;

    // TEMP Logger
    DaemonLogger m_daemonLogger;

    ConfigManager *m_configManager;
    QJsonDocument buildMessage(QJsonObject dataObj, QString type);
};

#include <QCoreApplication>
#include <QThread>
#include <QTimer>

#include <iostream>

class Console : public QObject
{
    Q_OBJECT
public:

public slots:
    void run()
    {
        using std::cin;
        std::string command;
        while(true){
            qDebug() << "Commands:\nquit to exit RFIDMonitorDaemon\nkick to stop RFIDMonitor\nrestart to restart RFIDMonitor";
            std::cin >> command;
            if(command == "quit"){
                emit exitApp();
                break;
            }else if(command == "kick"){
                emit sendMessage(QByteArray("ExitSystem"));
            }else if(command == "restart"){
                emit sendMessage(QByteArray("RestartSystem"));
            }else{
                qDebug() << QString::fromStdString(std::string("Invalid command \"") + command + std::string("\"\n"));
                QString random(QString::fromStdString(std::string("Random message: ") + command + std::string("\n")));
                emit sendMessage(QByteArray().append(random));
                //                emit sendMessage(QString::fromStdString(std::string("Random message: ") + command + std::string("\n")));
            }
        }
    }

signals:
    void sendMessage(const QByteArray &);
    void exitApp();
};

#endif // RFIDMONITORDAEMON_H
