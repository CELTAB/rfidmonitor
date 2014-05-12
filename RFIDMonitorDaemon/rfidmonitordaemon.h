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
#include <QLocalSocket>

#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QUdpSocket>
#include <QTimer>

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
        if (!m_daemonLogs.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){

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
    /**
     * @brief ipcNewConnection is called when the RFID Monitor is establishing connection. When some message arrive by this connection the routeIpcMessage function is called.
     *
     * @see routeIpcMessage();
     */
    void ipcNewConnection();

    /**
     * @brief ipcSendMessage  is responsible for send message to RFIDMonitor. Receive the message by parameter.
     * The message must to be in right format defined in protocol
     *
     * @param message to be sended to RFIDMonitor via IPC connection
     * @see buildMessage();
     */
    void ipcSendMessage(const QByteArray &message);

    /**
     * @brief tcpConnect is responsible for establish connection with the server. The IP address and port is defined in the configuration file.
     * When a connection is established the tcpConnected() function is called to make the handshake.
     *
     * @see tcpConnected()
     */
    void tcpConnect();

    /**
     * @brief When a connection is established this function is called to sends a SYN message to the server to accomplish the hasnshake (see protocol definition).
     *
     * @see tcpSendMessage();
     */
    void tcpConnected();

    /**
     * @brief tcpDisconnected is called when the connection with server is lost. In this case, after few seconds it will try to reconnect and then again until have success.
     * When accomplish a new connection the tcpConnected() function is called.
     *
     * @see tcpConnected()
     */
    void tcpDisconnected();

    /**
     * @brief tcpHandleError is called when any error occur in any time during the communication with the server.
     * @param error
     */
    void tcpHandleError(QAbstractSocket::SocketError error);

    void icpHandleError(QLocalSocket::LocalSocketError error);

    /**
     * @brief tcpSendMessage send messages to the tcp socket received by parameter. Receives the tcp socket because can send message to more then one tcp connection
     *
     * @param con TCP socket to send message for
     * @param message is the bytearray to be sended
     */
    void tcpSendMessage(QTcpSocket *con, const QByteArray &message);

    /**
     * @brief readDatagrams is waiting for UDP messages. When receives one UDP message it try to connect to the sender of this package.
     * This happens because the Daemon can be connected with the deskApp and the server simultaneously.
     * The Daemon knows the address of the server, but the address of the deskApp it receives by UDP message
     */
    void readDatagrams();

    /**
     * @brief routeTcpMessage is responsible for interpret the message that arrived by TCP and send to the right function responsible for interpret the data to this type of message.
     * If the reouter don't undestand the message type, respond with and ACK-UNKNOWN message. (see protocol definition)
     */
    void routeTcpMessage();

    /**
     * @brief routeIpcMessage is responsible for interpret the message that arrived by UDP and send to the right function responsible for interpret the data to this type of message.
     * If the reouter don't undestand the message type, respond with and ACK-UNKNOWN message. (see protocol definition)
     */
    void routeIpcMessage();

    void initMonitor();

signals:
    void restartMonitor();

private:
    bool isConnected;

    QLocalServer *m_localServer;
    QLocalSocket *ipcConnection;

    QProcess *m_process;
    QTimer *m_restoreTimer;

    // Node.js Server
    QTcpSocket *m_tcpSocket;
    QString m_serverName;
    QString m_hostName;
    int m_tcpPort;

    // Desktop Application Only
    QTcpSocket *m_tcpAppSocket;
    QUdpSocket *m_udpSocket;

    // TEMP Logger
    DaemonLogger m_daemonLogger;

    ConfigManager *m_configManager;

    /**
     * @brief buildMessage is used to build a JSON document based on protocol definition. The protocol looks like this:
     * \code
     * {
     *  "data": {
     *          },
     *  "type":"SYN",
     *  "datetime":"2014-04-10T09:23:28"
     * }
     * \endcode
     * The data field correspond to the information of the massage. The type field defines why the message was made for and the datetime field is a time stamp for logging purpose.
     * To know which type is used and why or when each one is used, refer to documentation.
     *
     * @param dataObj and type: Receive a QJsonObject to be inserted in data field and a message type to insert in the type field.
     * @return Reutur the messaga package in the right format defined by protocol.
     */
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
