#include "datapacket.h"


bool operator==(RfidData &d1, RfidData &d2)
{
    return d1.id == d2.id;
}


bool operator==(const RfidData &d1, const RfidData &d2)
{
    return d1.id == d2.id;
}


quint64 get_date(const QDateTime &dateTime)
{
    quint64 dt = 0;
    quint32 julianDays = dateTime.date().toJulianDay();
    quint32 milliseconds = dateTime.time().msecsSinceStartOfDay();

    memcpy(&dt, &julianDays, sizeof(quint32));
    memcpy(&((char *)&dt)[4], &milliseconds, sizeof(quint32));

    return dt;
}


QDateTime get_date(quint64 dateTime)
{
    quint32 julianDays = (dateTime << sizeof(quint32)*8) >> sizeof(quint32)*8;
    quint32 milliseconds = (dateTime >> sizeof(quint32)*8);

    QDateTime dt(QDate::fromJulianDay(julianDays), QTime::fromMSecsSinceStartOfDay(milliseconds));

    return dt;
}
