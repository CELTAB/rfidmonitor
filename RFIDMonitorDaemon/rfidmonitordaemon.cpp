/****************************************************************************
**
** WWW.FISHMONITORING.COM.BR
**
** Copyright (C) 2013
**                     Luis Valdes <luisvaldes88@gmail.com>
**                     Thiago Bitencourt <thiago.mbitencourt@gmail.com>
**                     Gustavo Valiati <gustavovaliati@gmail.com>
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
    m_tcpSocket(0),
    m_tcpAppSocket(0)
{
    m_localServer = new QLocalServer(this);
    m_tcpSocket = new QTcpSocket(this);
    m_tcpSocket->setObjectName("server");

    m_udpSocket = new QUdpSocket(this);
    m_tcpAppSocket = new QTcpSocket(this);
    m_tcpAppSocket->setObjectName("deskApp");

    m_serverName = "RFIDMonitorDaemon";

    m_configManager = new ConfigManager(this);

    connect(m_localServer, SIGNAL(newConnection()), SLOT(ipcNewConnection()));

    connect(m_tcpSocket, SIGNAL(connected()), SLOT(tcpConnected()));
    connect(m_tcpSocket, SIGNAL(disconnected()), SLOT(tcpDisconnected()));
    connect(m_tcpSocket, SIGNAL(readyRead()), SLOT(routeTcpMessage()));
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(tcpHandleError(QAbstractSocket::SocketError)));

    connect(m_udpSocket, SIGNAL(readyRead()), SLOT(readDatagrams()));

    connect(m_tcpAppSocket, &QTcpSocket::connected, ([=] () { m_udpSocket->close(); m_daemonLogger <<  "Connected with DeskApp";}));
    connect(m_tcpAppSocket, &QTcpSocket::disconnected,
            ([=] () {
        m_daemonLogger <<  "Connection Closed";
        if(!m_udpSocket->bind(QHostAddress::Any, 9999)){
            m_daemonLogger <<  QString("Couldn't listening broadcast");
        };
    }));

    connect(m_tcpAppSocket, SIGNAL(readyRead()), SLOT(routeTcpMessage()));

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

        // consoleThread->start();

        QTimer::singleShot(100, this, SLOT(tcpConnect()));
    }else{
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "Could not listen!";
#endif
    }
    if(!m_udpSocket->bind(QHostAddress::Any, 9999))
        m_daemonLogger <<  QString("Couldn't listening broadcast");
}

void RFIDMonitorDaemon::ipcNewConnection()
{
#ifdef DEBUG_LOGGER
    m_daemonLogger <<  "New client connected.";
#endif
    ipcConnection = m_localServer->nextPendingConnection();

    connect(ipcConnection, SIGNAL(readyRead()), SLOT(routeIpcMessage()));
    connect(ipcConnection, &QLocalSocket::disconnected,
            [this]() {
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "Client Disconnected";
#endif
    });
    connect(ipcConnection, SIGNAL(disconnected()), ipcConnection, SLOT(deleteLater()));
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
    tcpSendMessage(m_tcpSocket, buildMessage(m_configManager->identification(), "SYN").toJson());
}

void RFIDMonitorDaemon::tcpDisconnected()
{
    QTimer::singleShot(5000, this, SLOT(tcpConnect()));
}

void RFIDMonitorDaemon::tcpHandleError(QAbstractSocket::SocketError error )
{
    if(error == QAbstractSocket::ConnectionRefusedError)
        tcpDisconnected();

#ifdef DEBUG_LOGGER
    m_daemonLogger <<  QString("Error: %1 - %2").arg(m_tcpSocket->error()).arg(m_tcpSocket->errorString());
#endif
}

void RFIDMonitorDaemon::tcpSendMessage(QTcpSocket *con, const QByteArray &message)
{
#ifdef DEBUG_LOGGER
    m_daemonLogger <<  QString("Message size: %1").arg(message.size());
#endif
    if(con->isOpen()){
        //PACKAGE SIZE
        QByteArray data;
        QString packageSizeStr(QString::number(message.size()));
        data.fill('0', sizeof(quint64) - packageSizeStr.size());
        data.append(packageSizeStr);
        data.append(message);

        con->write(data);
        con->flush();
    } else {
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "Doesn't work mada faka";
#endif
    }
}

void RFIDMonitorDaemon::ipcSendMessage(const QByteArray &message)
{
    if(ipcConnection)
        ipcConnection->write(message);
}

/*
 * Build a message to be sent. Uses the right format defined by protocol
 * Receive a QJsonObject to be inserted in data field and a message type to insert in the type field
 */
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

/*
 * The routeTcpMessage() receives messages from any TCP connection. The Daemon can be connected with server and deskApp at the same time, but it will know which connection received data.
 * Any message arrive by TCP must to have two parts. The first part defines the data size of the coming package (second part) and must to be an 8 bytes String (always 8 bytes).
 *
 * The algoritm works like this:
 * When some data arrive it verifies (using the hasPackage variable) if is the first part or the second one of the complete message;
 * If hasPackage is true then is especting the second part of the message, otherwise the dada just arrived is the size information.
 *
 * The information of size correspond to the size of the message package. So, when more data arrive it verifies if the size is greater than or equals to the expected size.
 * If true the message is all here and can go on, otherwise the message is coming and it will wait for more data.
 *
 * When all the message arrives it will interpret and route it (by message type) to the right way.
 *
 */
void RFIDMonitorDaemon::routeTcpMessage()
{
    QTcpSocket *connection = (QTcpSocket *) QObject::sender();

    static bool hasPackage = false;
    static quint64 packageSize = 0;

    if( ! hasPackage){
        m_daemonLogger <<  "waiting for package";

        if((quint64)connection->bytesAvailable() < sizeof(quint64))
            return;

        //    	m_tcpSocket->read((char *)&packageSize, sizeof(quint64));
        QString packageSizeStr(connection->read(sizeof(quint64)));
        packageSize = packageSizeStr.toULongLong();

        m_daemonLogger <<  QString("Message = %1 - Size of coming package: %2").arg(packageSizeStr).arg(QString::number(packageSize));
        hasPackage = true;
    }

    if((quint64)connection->bytesAvailable() >=  packageSize){
        QByteArray data(connection->read(packageSize));

        json::NodeJSMessage nodeMessage;

        nodeMessage.read(QJsonDocument::fromJson(data).object());
        QString messageType(nodeMessage.type());

        m_daemonLogger << QString("Node.js Message: %1").arg(QString(data));

        if(messageType == "ACK-SYN"){
            m_daemonLogger <<  "ACK-SYN Message";

            QJsonObject obj(nodeMessage.jsonData());
            if(!obj.isEmpty()){
                m_configManager->setIdentification(obj);
            }

            bool statusDateTime = m_configManager->setDateTime(nodeMessage.dateTime());
            QJsonObject response = m_configManager->identification();
            response["success"] = QJsonValue(statusDateTime);

            tcpSendMessage(connection, buildMessage(response, "ACK").toJson());

        }else if (messageType == "GET-CONFIG") {
            m_daemonLogger <<  "GET-CONFIG Received";
            tcpSendMessage(connection, buildMessage(m_configManager->currentConfig(), "CONFIG").toJson());

        }else if (messageType == "READER-COMMAND") {
            m_daemonLogger <<  "READER-COMMAND Received";

            QJsonObject command(nodeMessage.jsonData());
            //        	m_daemonLogger <<   QString(QJsonDocument(command).toJson());

            /*
             * When a 'reader command' message is received is because someone is sending a command to the reader. So it needs to send also who is doing this.
             * To perform this it uses a field called 'sender' that carry the name of who is sending the 'command message'.
             * And based on that, it will respond to the server connection or to the deskApp connection
             *
             * see reoutIcpMessage (messageType == "READER-RESPONSE")
             */
            if(connection->objectName() == "server")
                command.insert("sender", QString("server"));
            else
                command.insert("sender", QString("app"));

            m_daemonLogger <<  QString(QJsonDocument(command).toJson());

            ipcSendMessage(buildMessage(command, "READER-COMMAND").toJson());

        }else if (messageType == "NEW-CONFIG") {
            m_daemonLogger <<  "NEW-CONFIG Received";

            QJsonObject newConfig(nodeMessage.jsonData());
            bool ackConf;
            if(m_configManager->newConfig(newConfig))
            {
                // ADICIONAR VERIFICAÇÃO DE REINICIALIZAÇÃO DO MONITOR
                /*
            * Quando um novo arquivo de configuração for recebido e persistido o monitor deve ser reiniciado.
            * Enquanto o aplicativo reiniciar o 'sender' esta esperando por uma configuração.
            * Deve ser enviado uma mensagem de sucesso ou fracasso na inicializaão com as novas configurações.
            *
            * Se o monitor não conseguir iniciar com as novas configurações deve ser resetado para as configurações anteriores.
            * Portanto, deve ser mantido um arquivo de configurações de backup temporario.
            */
                ackConf = true;
            }else{
                ackConf = false;
            }
            QJsonObject dataObj;
            dataObj.insert("success", QJsonValue(ackConf));
            tcpSendMessage(connection, buildMessage(dataObj, "ACK-NEW-CONFIG").toJson());

        }else if (messageType == "DATETIME") {
            m_daemonLogger <<  "DATETIME Received";

            QJsonObject dataObj;
            dataObj["success"] =  QJsonValue(m_configManager->setDateTime(nodeMessage.dateTime()));//? QString("Date/Time update successfully") : QString("Error to update Date/Time"));
            tcpSendMessage(connection, buildMessage(dataObj, "ACK").toJson());

        }else if (messageType == "ACK-DATA") {
            // A ACK-DATA message means that the server is trying to inform the RFIDMonitor that some data is now synced. So, it just send this message to the RFIDMonitor.
            ipcSendMessage(data);

            // DEBUG PURPOSE -- SHOW THE MD5 DIGGEST
            QJsonObject ackData(nodeMessage.jsonData());
            QString hash = ackData["md5diggest"].toString();
            m_daemonLogger <<  "The package was successfulle synced. PACKAGE HASH: " << hash;

        }else if (messageType == "GET-NET-CONFIG") {
            // Only return the network configuration.
            tcpSendMessage(connection, buildMessage(m_configManager->netConfig(), "NET-CONFIG").toJson());

        }else if (messageType == "NEW-NET") {
            m_daemonLogger <<  "NEW-NET Received";
            QJsonObject network = nodeMessage.jsonData();

            // Receive a new configuration for the network (ssid and password).
            QJsonObject dataObj;
            dataObj.insert("success", QJsonValue(m_configManager->setNetConfig(network)));

            // returns a message ACK-NET to inform the sender that the new configuration was set
            tcpSendMessage(connection, buildMessage(dataObj, "ACK-NET").toJson());

            // Try to restar the network service to already use the new configuration
            bool resetNet = m_configManager->restartNetwork();
            m_daemonLogger <<  QString(resetNet? "Network restarted" : "Networkt Don't restarted");

        }else if (messageType == "ACK-UNKNOWN") {
            QJsonDocument unknown(nodeMessage.jsonData());
            QJsonObject oldMessage(unknown.object().value("unknownmessage").toObject());
            m_daemonLogger <<  "The server don't understand the message type: " << oldMessage.value("type").toString();
            m_daemonLogger <<  "ERROR message: " << unknown.object().value("errorinfo").toString();
        }
        else{
            /* When receives a message that can't be interpreted like any type is an unknown message.
             * In this case an ACK-UNKNOWN message is built and sent to the connection that received this message
             */
            m_daemonLogger <<  "UNKNOWN MESSAGE";
            QJsonObject unknownObj;
            unknownObj.insert("unknownmessage", QJsonValue(QJsonDocument::fromJson(data).object()));
            unknownObj.insert("errorinfo", QString("Unknown message received"));
            tcpSendMessage(connection, buildMessage(unknownObj, "ACK-UNKNOWN").toJson());
        }

        /* when all the process is done, reset the expecting message size to zero and the haspackage to false.
         * Then when more data arrive it must to be the size information again.
         */
        packageSize = 0;
        hasPackage = false;
    }
}

/*
 * This function works pretty much like routTcpMessage only that this one interpretes message from RFIDMonitor and don't verify packages size.
 * it only tries to interpret data just arrived.
 */
void RFIDMonitorDaemon::routeIpcMessage()
{
    QByteArray message = ipcConnection->readAll();
    json::NodeJSMessage nodeMessage;

    nodeMessage.read(QJsonDocument::fromJson(message).object());
    QString messageType(nodeMessage.type());

#ifdef DEBUG_LOGGER
    m_daemonLogger <<  QString("RFIDMonitorDaemon -> IPC Message received: %1").arg(QString(""));
#endif

    if(messageType == "SYN"){
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "SYN Message received from MONITOR";
#endif
        ipcSendMessage(buildMessage(QJsonObject(), "ACK-SYN").toJson());

    }else if (messageType == "READER-RESPONSE") {
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "READER-RESPONSE Received";
#endif
        QJsonObject command(nodeMessage.jsonData());
        /*
         * When a 'reader response' message is received is because someone sent a 'reader command' message. So it needs to know who did it.
         * To perform this it uses a field called 'sender' that carry the name of who sent the 'command message'.
         * And based on that, it will respond to the server connection or to the deskApp connection.
         *
         * see reoutTcpMessage (messageType == "READER-COMMAND")
         */
        if(command.value("sender").toString() == "server")
            tcpSendMessage(m_tcpSocket, message);
        else
            tcpSendMessage(m_tcpAppSocket, message);

    }else if (messageType == "DATA"){
        /*
         * A Data message means that the RFIDMonitor is trying to sync some data into the server. So, it just send this message to the server.
         */
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "DATA Received\n";
        m_daemonLogger <<  QString(QJsonDocument(nodeMessage.jsonData()).toJson());
#endif
        tcpSendMessage(m_tcpSocket, message);//buildMessage(nodeMessage.jsonData(), "DATA").toJson());

    }else if (messageType == "ACK-UNKNOWN") {
        QJsonDocument unknown(nodeMessage.jsonData());
        QJsonObject dataObj(unknown.object().value("unknownmessage").toObject());
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "The Monitor don't understand the message type: "
                        << dataObj.value("type").toString();
#endif
    }
    else{
        /* When receives a message that can't be interpreted like any type is an unknown message.
         * In this case an ACK-UNKNOWN message is built and sent to the connection that received this message
         */
#ifdef DEBUG_LOGGER
        m_daemonLogger <<  "UNKNOWN MESSAGE";
#endif
        QJsonObject unknownObj;
        unknownObj.insert("unknownmessage", QJsonDocument::fromJson(message).object());
        unknownObj.insert("errorinfo", QString("Unknown message received"));

        ipcSendMessage(buildMessage(unknownObj, "ACK-UNKNOWN").toJson());
    }
}

void RFIDMonitorDaemon::readDatagrams()
{
    QByteArray datagram;
    datagram.resize(m_udpSocket->pendingDatagramSize());
    QHostAddress sender;

    m_udpSocket->readDatagram(datagram.data(), datagram.size(), &sender);

    QJsonDocument broacast(QJsonDocument::fromJson(datagram));
#if QT_VERSION < 0x050200
    m_tcpAppSocket->connectToHost(sender, broacast.object().value("port").toVariant().toInt());
#else
    m_tcpAppSocket->connectToHost(sender, broacast.object().value("port").toInt());
#endif
    if(m_tcpAppSocket->waitForConnected())
        tcpSendMessage(m_tcpAppSocket, buildMessage(m_configManager->identification(), "SYN").toJson());
    else
        m_daemonLogger <<  QString("Could not connect with %1").arg(sender.toString());
}
