#include <QCoreApplication>
#include <QDebug>

#include "datapacket.h"
#include "tcpclient.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QDateTime now(QDateTime::currentDateTime());
    quint64 dateTime = 0;
    quint32 julianDays = now.date().toJulianDay();
    quint32 milliseconds = now.time().msecsSinceStartOfDay();
    char *buf = (char *)&(dateTime);
    memcpy(&dateTime, &julianDays, sizeof(quint32));

    buf = &buf[4];
    memcpy(&((char *)&dateTime)[4], &milliseconds, sizeof(quint32));

    quint32 julianDays2 = (dateTime << sizeof(quint32)*8) >> sizeof(quint32)*8;
    quint32 milliseconds2 = (dateTime >> sizeof(quint32)*8);

    int x = 1;
    int result = x << 1;
    qDebug() << QString("x = %1 & result = %2").arg(x).arg(result);
    qDebug() << "Size of RfidData = " << sizeof(RfidData);
    qDebug() << "Size of DataPacket = " << sizeof(DataPacket);
    qDebug() << "Size of tDataPacket = " << sizeof(tDataPacket);

    TcpClient client;
    client.start();
    //client.sendRandomData();
//    client.sendRandomDataNewDataPacket();



    return a.exec();
}
