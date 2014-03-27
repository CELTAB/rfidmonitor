#include <QTcpSocket>
#include <QHostAddress>
#include <cmath>
#include <stdlib.h>
#include <QTimer>

#include "tcpclient.h"
#include "datapacket.h"

TcpClient::TcpClient(QObject *parent) :
    QObject(parent)
{
    m_socket = new QTcpSocket(this);
    connect(m_socket, SIGNAL(readyRead()), SLOT(readyRead()));
    connect(m_socket, SIGNAL(connected()), SLOT(connected()));
    connect(m_socket, SIGNAL(disconnected()), SLOT(disconnected()));
    connect(m_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), SLOT(handleStateChanged(QAbstractSocket::SocketState)));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(handleError(QAbstractSocket::SocketError)));
}

void TcpClient::start()
{
    m_socket->connectToHost("localhost", 9999);
}

void TcpClient::sendRandomData()
{
    srand(time_t(NULL));

    DataPacket packet;
    packet.idBlock = 14;
    packet.startId = rand() % 10500 + 1000;
    packet.endId = rand() % 10500 + 1000;
    packet.idReading = rand() % 10500 + 1000;
    packet.size = 0;
    memset(packet.data, 0, PACKET_DATA_SIZE);

    QList<RfidData> randomData;

    for(int i = 0; i < 400; i++) {
        RfidData data;
        data.id = i + 1;
        data.idpontocoleta = rand() % 10 + 1;
        data.idantena = rand() % 3 + 1;
        data.identificationcode = rand() % 1000 + 1;
        data.applicationcode = rand() % 10000000 + 250000;
        data.dateTime = get_date(QDateTime::currentDateTime());
        randomData.append(data);
    }

    packet.size = randomData.size();

    foreach (const RfidData &d, randomData) {
        int index = randomData.indexOf(d);

        memcpy(&(packet.data[index*sizeof(RfidData)]), (char *)&d, sizeof(RfidData));
        char retrievied[64];
        memcpy(retrievied, &(packet.data[index*sizeof(RfidData)]), sizeof(RfidData));

        RfidData *rt1 = reinterpret_cast<RfidData *>(retrievied);
        if(rt1){
            QDateTime dateTime(get_date(rt1->dateTime));
            qDebug() << QString("id = %1, appcode = %2, idcode = %3, datetime = %4").arg(rt1->id).arg(rt1->applicationcode).arg(rt1->identificationcode).arg(dateTime.toString());
        }
    }

    if(m_socket){
        qint64 bytes = m_socket->write((char *)&packet, (qint64)sizeof(DataPacket));
        qDebug() << QString("%1 of %2 bytes written to server.").arg(bytes).arg((qint64)sizeof(DataPacket));
        QTimer::singleShot(10000, this, SLOT(sendRandomData()));
    }
}

QString state_string(int socketState){
    QString state;
    switch (socketState) {
    case QAbstractSocket::UnconnectedState:
        state = "QAbstractSocket::UnconnectedState";
        break;
    case QAbstractSocket::HostLookupState:
        state = "QAbstractSocket::HostLookupState";
        break;
    case QAbstractSocket::ConnectingState:
        state = "QAbstractSocket::ConnectingState";
        break;
    case QAbstractSocket::ConnectedState:
        state = "QAbstractSocket::ConnectedState";
        break;
    case QAbstractSocket::BoundState:
        state = "QAbstractSocket::BoundState";
        break;
    case QAbstractSocket::ListeningState:
        state = "QAbstractSocket::ListeningState";
        break;
    case QAbstractSocket::ClosingState:
        state = "QAbstractSocket::ClosingState";
        break;
    default:
        break;
    }

    return state;
}


void TcpClient::sendRandomDataNewDataPacket()
{
    int state = m_socket->state();
    QString statestr = state_string(state);



    if(m_socket->state() == QAbstractSocket::ConnectedState){
        srand(time_t(NULL));

        tDataPacket packet;
        packet.flags = KData;
        packet.idsender = rand() % 10 + 1;
        memcpy(packet.name, "CANAL PIRACEMA", strlen("CANAL PIRACEMA"));
        packet.dateTime = get_date(QDateTime::currentDateTime());
        packet.temperature = 29.6;
        packet.dataSize = 0;

        QList<RfidData> randomData;

        int quantity = rand() % 900 + 125;
        qDebug() << QString("Quantity = %1").append(quantity);

        for(int i = 0; i < quantity; i++) {
            RfidData data;
            data.id = i + 1;
            data.idpontocoleta = rand() % 10 + 1;
            data.idantena = rand() % 3 + 1;
            data.identificationcode = rand() % 1000 + 1;
            data.applicationcode = rand() % 10000000 + 250000;
            data.dateTime = get_date(QDateTime::currentDateTime());
            randomData.append(data);
        }

        packet.dataSize= randomData.size();

        quint64 bufferSize = sizeof(tDataPacket) + randomData.size()*sizeof(RfidData);
        char dataBuffer[bufferSize];

        // save the DataPacket
        memcpy(dataBuffer, &packet, sizeof(tDataPacket));

        int offset = sizeof(tDataPacket);
        foreach (const RfidData &d, randomData) {
            int index = randomData.indexOf(d);

            memcpy(&(dataBuffer[offset + index*sizeof(RfidData)]), (char *)&d, sizeof(RfidData));
            char retrievied[64];
            memcpy(retrievied, &(dataBuffer[offset + index*sizeof(RfidData)]), sizeof(RfidData));

            RfidData *rt1 = reinterpret_cast<RfidData *>(retrievied);
            if(rt1){
                QDateTime dateTime(get_date(rt1->dateTime));
                qDebug() << QString("id = %1, appcode = %2, idcode = %3, datetime = %4").arg(rt1->id).arg(rt1->applicationcode).arg(rt1->identificationcode).arg(dateTime.toString());
            }
        }

        if(m_socket){
            qint64 bytes = m_socket->write(dataBuffer, bufferSize);
            qDebug() << QString("%1 of %2 bytes written to server.").arg(bytes).arg(bufferSize);
            QTimer::singleShot(10000, this, SLOT(sendRandomDataNewDataPacket()));
        }
    }
}

void TcpClient::sendRandomClientPacket()
{
    /*
        KRequest = 0x55000000,
        KSet = 0xAA000000,
        KData = 0x4,
        KTemperature = 0x8,
        KDateTime = 0x10,
        KIPAddress = 0x20,
        KName = 0x40,
        KMAC = 0x80,
        KID = 0x100,
        KTimeOuts = 0x200,
        KSynchronizeAll = 0x400
    */
    // 9 items
    int messageType[] = {0x55000000, 0xAA000000};
    int clientFlags[] = {0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400};

    int index = rand() % 2;

    if((messageType[index] & KRequest) == KRequest){

    }else{

    }
}

void TcpClient::connected()
{
    sendRandomDataNewDataPacket();
}

void TcpClient::disconnected()
{
    qDebug() << "Disconnected";
    QTimer::singleShot(10000, this, SLOT(start()));
}

void TcpClient::handleStateChanged(QAbstractSocket::SocketState socketState)
{
    QString state = state_string(socketState);
    qDebug() << QString("State %1: %2").arg(socketState).arg(state);
}

void TcpClient::handleError(QAbstractSocket::SocketError socketError)
{
    if(m_socket->state() == QAbstractSocket::UnconnectedState){
        qDebug() << "Disconnected from server, trying again in 10 seconds.";
        qDebug() << QString("Error %1: %2").arg(socketError).arg(m_socket->errorString());
        //        QTimer::singleShot(10000, this, SLOT(start()));
    }
}

void TcpClient::readyRead()
{
    QByteArray received = m_socket->readAll();

    char buffer[sizeof(ServerPacketHeader)];
    memcpy(buffer, received.data(), sizeof(ServerPacketHeader));
    ServerPacketHeader *serverHeader = reinterpret_cast<ServerPacketHeader *>(buffer);
    if(serverHeader) {
        /*
            KRequest = 0x55000000,
            KSet = 0xAA000000,
            KData = 0x4,
            KTemperature = 0x8,
            KDateTime = 0x10,
            KIPAddress = 0x20,
            KName = 0x40,
            KMAC = 0x80,
            KID = 0x100,
            KTimeOuts = 0x200,
            KSynchronizeAll = 0x400
        */
        if((serverHeader->flags & KRequest) == KRequest){
            // The server sent a message that needs to be answered with some information
            int option = serverHeader->flags &~ KRequest;
            switch (option) {
            case KData:
                // schedule synchronization of data
                break;
            case KTemperature:
                // schedule to send temperature
                break;
            case KDateTime:
                // Update time of the Reading Point
                break;
            case KIPAddress:
                // Send IP Address
                break;
            case KName:
                // Send Name
                break;
            case KMAC:
                // Send MAC Address
                break;
            case KID:
                // Change ID of the Reading Point
                break;
            case KTimeOuts:
                // Change Timeouts for Reading and Synchronization
                break;
            case KSynchronizeAll:
                // Synchronize With Server
                break;
            default:
                qDebug() << "Unknown request";
                break;
            }
        }else if ((serverHeader->flags & KSet) == KSet){
            // The server sent a message that contains settings for the Collection Point
            int option = serverHeader->flags &~ KSet;
            switch (option) {
            case KDateTime:
                // Update time of the Reading Point
                qDebug() << "Execute Gustavo's Script to Update the time of the Collection Point";
                break;
            case KName:
                qDebug() << "Modify the name of the CP";
                break;
            case KID:
                qDebug() << "Modify the ID of the CP";
                break;
            case KTimeOuts:
                qDebug() << "Modify the TimeOuts for Reading and Synchronization";
                break;
            case KIPAddress:
            case KMAC:
            case KData:
            case KTemperature:
            case KSynchronizeAll:
                break;
            default:
                qDebug() << "Unknown request";
                break;
            }
        }else{
            // The validation bits are invalid
            qDebug() << "The Validation bits are corrupted";
        }
    }
}
