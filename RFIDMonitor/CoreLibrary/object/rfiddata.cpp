// ****************************************************
// * Codigo Generado por DAOBuilder                   *
// * Fecha: 30/07/2013 10:33:09                       *
// * Luis Valdes 2012 - Todos los Derechos Reservados *
// ****************************************************

#include <QSqlRecord>

#include "rfiddata.h"

Rfiddata::Rfiddata(QObject *parent) :
    QObject(parent)
{

}

Rfiddata::Rfiddata(const QSqlRecord &record, QObject *parent) :
    QObject(parent)
{
    // id, rfidcode, extra_data, datetime, sync
	setId(record.value(0));
    setRfidcode(record.value(1));
    setExtraData(QJsonDocument::fromJson(record.value(2).toString().toUtf8()).object());
    setDatetime(record.value(3));
    setSync(record.value(4));
}

QVariant Rfiddata::id() const
{
    return m_id;
}

void Rfiddata::setId(QVariant value)
{
    m_id = value.toLongLong();
}

QVariant Rfiddata::rfidcode() const
{
    return m_rfidcode;
}

void Rfiddata::setRfidcode(QVariant value)
{
    m_rfidcode = value.toLongLong();
}

QVariant Rfiddata::datetime() const
{
	return m_datetime;
}

void Rfiddata::setDatetime(QVariant value)
{
    m_datetime = value.toDateTime();
}

QJsonObject Rfiddata::extraData() const
{
    return m_extradata;
}

void Rfiddata::setExtraData(QJsonObject value)
{
    m_extradata = value;
}

QVariant Rfiddata::sync() const
{
    return m_sync;
}

void Rfiddata::setSync(QVariant value)
{
    m_sync = (SyncState) value.toInt();
}
