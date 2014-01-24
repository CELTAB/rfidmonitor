#ifndef DATAPACKET_H
#define DATAPACKET_H

#include <QObject>
#include <QDateTime>
#include <QByteArray>

/*
QDate
            Julian day (quint32)
QTime
            Milliseconds since midnight (quint32)
QDateTime
            Date (QDate)
            Time (QTime)
            0 for Qt::LocalTime, 1 for Qt::UTC (quint8)
*/


// 48 bytes + 16 padding
struct RfidData
{
    qlonglong id;
    qlonglong idantena;
    qlonglong idpontocoleta;
    qlonglong applicationcode;
    qlonglong identificationcode;
    quint64 dateTime;
    char padding[16];
};

const int PACKET_DATA_SIZE = 64*1024;

struct DataPacket
{
    int idReading;
    int idBlock;
    qlonglong startId;
    qlonglong endId;
    qlonglong size;
    char data[PACKET_DATA_SIZE];
};

bool operator==(RfidData &d1, RfidData &d2);
bool operator==(const RfidData &d1, const RfidData &d2);

/*
0x1     Request
0x2     Set
0x4     Data
0x8     Temperature
0x10    Date & Time
0x20    IP Address
0x40	Name
0x80	MAC
0x100	ID
0x200	TimeOuts
0x400	Synchronize All
*/

enum Flags
{
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
};

typedef struct sDataPacket
{
    quint32	flags;
    quint32	idsender;
    char name[24];
    float temperature;
    quint64 dateTime;
    union {
        struct {
            quint64	ipAddress;
        } IP_Address;

        struct {
            char macAddress[6];
            char padding[2];
        } MAC_Address;

        struct {
            quint32	julianDays;
            quint32	milliseconds;
        } DateTime;

        struct {
            quint32	readerTimeOut;
            quint32	synTimeOut;
        } TimeOuts;

    } MessageUnion;
    quint32	dataSize;
} tDataPacket;


quint64 get_date(const QDateTime &dateTime);
QDateTime get_date(quint64 dateTime);


// Header structures for packets sent from Server to Client
struct ServerPacketHeader
{
    quint32	flags;
    quint64 dateTime;
    union {
        struct {
            quint64	id;
            char clientName[24];
        } ClientInfo;

        struct {
            quint32	readerTimeOut;
            quint32	synTimeOut;
        } TimeOuts;
    } MessageUnion;
};


// Header structures for packets sent from Client to Server
struct ClientPacketHeader
{
    quint32	flags;
    quint32	idsender;
    char name[24];
    float temperature;
    quint64 dateTime;
    union {
        struct {
            quint64	ipAddress;
        } IP_Address;

        struct {
            char macAddress[6];
            char padding[2];
        } MAC_Address;

        struct {
            quint32	readerTimeOut;
            quint32	synTimeOut;
        } TimeOuts;

        quint32	dataSize;
    } MessageUnion;
};

#endif // DATAPACKET_H
