#ifndef NETWORKCOMMUNICATION_H
#define NETWORKCOMMUNICATION_H

#include <QObject>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QByteArray>
#include <QAbstractSocket>
#include <QTimer>
#include <QMap>
#include <QJsonObject>

#include "settings.h"

class NetworkCommunication : public QObject
{
    Q_OBJECT

private:
    explicit NetworkCommunication(QObject *parent = 0);

    /**
     * @brief m_udpSocket holds the instance of QUdpSocket to work with the udp broadcast.
     */
    QUdpSocket *m_udpSocket;

    /**
     * @brief m_tcpServer holds the instance of the TCP server that waits connections from rasps.
     */
    QTcpServer *m_tcpServer;

    /**
     * @brief m_tcpSocket holds the instance QTcpSocket, of the current main connection to the rasp.
     */
    QTcpSocket *m_tcpSocket;

    /**
     * @brief m_udpPort determines in which port the broadcast will be emmited.
     */
    int m_udpPort;

    /**
     * @brief m_localAddress holds the local IP Address found in the current machine, to inform in the
     * broadcast which of available IP must receive connections from rasps.
     */
    QString m_localAddress;

    /**
     * @brief m_tcpPort holds the TCP port choosen automatically by the QTcpServer.
     */
    int m_tcpPort;

    /**
     * @brief m_udpTimer holds the instance of QTimer that determine when the UDP broadcast
     * must be emmited in a predefined interval.
     */
    QTimer *m_udpTimer;

    /**
     * @brief m_tcpSocketMap map all the raps that have successfully connected to the TCP server,
     * and did the hanshake correctly.
     */
    QMap<QString, QTcpSocket*> *m_tcpSocketMap;

    /**
     * @brief handshakeACK handle the handshake with the rasp, with a ACK answer from it.
     * @param socket is the connection where the message came from.
     * @param jsonObject is the data of the message.
     */
    void handshakeACK(QTcpSocket *socket, QJsonObject jsonObject);

    /**
     * @brief handshakeSYN handle the handshake with the rasp, with a SYN message from it.
     * @param socket is the connection where the message came from.
     * @param jsonObject is the data of the message.
     */
    void handshakeSYN(QTcpSocket *socket, QJsonObject jsonObject);

    /**
     * @brief raspConfigReceived handle the current configuration sent by the rasp.
     * @param jsonObject is the data of the message, with the configuration file in json format.
     */
    void raspConfigReceived(QJsonObject jsonObject);

    /**
     * @brief answerFromReader handle a new reader answer sent by rasp.
     * @param dataObj is the data of the message, with the reader answer.
     */
    void answerFromReader(QJsonObject dataObj);

    /**
     * @brief sendAckUnknown sends a message of the ACK-UNKNOWN to the socket.
     * @param socket is the connection where the message must be sent.
     * @param originalMessage is the full original message to give back to the sender.
     * @param errorInfo contains some info about why the message is being sent.
     */
    void sendAckUnknown(QTcpSocket *socket, QJsonObject originalMessage, QString errorInfo);

    /**
     * @brief ackUnknownReceived handles a ACK-UNKNOWM sent from a connection.
     * @param jsonObject is the data of the message, containing info about the problem.
     */
    void ackUnknownReceived(QJsonObject jsonObject);

    /**
     * @brief ackNewConfig handle the confirmation about the new configuration sent to rasp.
     * @param jsonObject is the data of message.
     */
    void ackNewConfig(QJsonObject jsonObject);

public:
    /**
     * @brief instance is the implementation of singleton pattern for this class.
     * @return the unique instance of NetworkCommunication class.
     */
    static NetworkCommunication *instance();

    /**
     * @brief startBroadcast start broadcasting with udp to inform the rasps where they
     * must connect.
     */
    void startBroadcast();

    /**
     * @brief stopBroadcast stop broadcasting.
     */
    void stopBroadcast();

    /**
     * @brief sendData send a message to a connection.
     * @param socket is the connection to send the message.
     * @param type is the type of the message.
     * @param data is the data of the message.
     */
    void sendData(QTcpSocket *socket, const QString &type, const QJsonObject &data);

    /**
     * @brief connectToRasp define which connection to use with the user's choice.
     *
     * Holds the connection, in m_tcpSocket, selected by the user in the GUI, and close all other
     * connections.
     *
     * @param ip defines which connection to use by the IP of the host.
     */
    void connectToRasp(const QString &ip);

    /**
     * @brief getConfigFromRasp send a message to the rasp to retrieve the current config.
     */
    void getConfigFromRasp();

    /**
     * @brief sendNewConfigToRasp send a new configuration to the rasp.
     * @param json is the new configuration in json format.
     */
    void sendNewConfigToRasp(const QJsonObject &json);

    /**
     * @brief sendNewCommandToReader send a message with a command to the reader.
     * @param command is the command to send.
     */
    void sendNewCommandToReader(const QString &command);

    /**
     * @brief closeTCPConnection closes the current main connection.
     */
    void closeTCPConnection();

    void sendFullRead(bool full);

private slots:
    /**
     * @brief tcpDataAvailable receives notification from QTcpSocket that exists new
     * bytes available from network.
     *
     * This function verify the message size, wait for the complete message, and process
     * it calling a specific function determined by the message type.
     */
    void tcpDataAvailable();

    /**
     * @brief tcpSocketError handles any error sent by the QTcpSocket.
     * @param socketError is the error detected by the QTcpSocket.
     */
    void tcpSocketError(const QAbstractSocket::SocketError socketError);

    /**
     * @brief newConnection handles a new TCP connection.
     *
     * Called when a new rasp tries to connect to the QTcpServer.
     */
    void newConnection();

    /**
     * @brief tcpDisconnected handle a disconnection from a TCP connection.
     *
     * The disconnection can refer to 3 different situations:
     *  - 1: A host didnt handshake correctly or in time, so it receives a signal to die,
     * and closes the connection.
     *  - 2: The user selected a host to maintain connection, and then disconnects all others
     * hosts that was waiting.
     *  - 3: The current main connection (selected by the user) is closed.
     */
    void tcpDisconnected();

    /**
     * @brief sendDatagram send a new broadcast message.
     *
     * This is called by the m_udpTimer timer many times with a interval.
     */
    void sendDatagram();

signals:

    /**
     * @brief raspDisconnected emmited when a connection is closed, sending the IP of it.
     * @param ipAddress is the IP of the disconnected host.
     */
    void raspDisconnected(QString ipAddress);

    /**
     * @brief newRaspFound emmited when a new rasp is successfully connected (with handshake).
     * @param raspInfo is a map of information about the new host.
     */
    void newRaspFound(QVariantMap raspInfo);

    /**
     * @brief currentConfigFromRasp emmited when there is a new config from rasp available.
     * @param json is the configuration in json format.
     */
    void currentConfigFromRasp(QJsonObject json);

    /**
     * @brief newReaderAnswer emmited when there is a new answer from rasp available.
     * @param answer is the answer.
     */
    void newReaderAnswer(QString answer);

    /**
     * @brief connectionEstablished emmited when the procedure to holds a main connection
     * to the host selected by the user, is successfully done.
     */
    void connectionEstablished();

    /**
     * @brief connectionFailed emmited when the current connection is closed by any reason.
     */
    void connectionFailed();

    /**
     * @brief currentConfigStatusFromRasp emmited to inform the status of setting the new
     * configuration file in the rasp.
     *
     * This is emmited when a new message of type ACK-NEW-CONFIG is received, that says
     * success or failure.
     *
     * @param obj contains the message with information about the status.
     */
    void currentConfigStatusFromRasp(QJsonObject obj);

};

#endif // NETWORKCOMMUNICATION_H
