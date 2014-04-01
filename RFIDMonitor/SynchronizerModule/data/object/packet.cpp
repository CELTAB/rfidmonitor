#include <QSqlRecord>

#include "packet.h"

Packet::Packet(QObject *parent) :
    QObject(parent)
{
}

Packet::Packet(const QSqlRecord &record, QObject *parent) :
    QObject(parent)
{
    // md5hash, datetime, idbegin, item_count, jsondata, status
    setMd5hash(record.value(0));
    setDateTime(record.value(1));
    setIdBegin(record.value(2));
    setItemCount(record.value(3));
    setJsonData(record.value(4));
    setStatus(record.value(5));
}

QVariant Packet::md5hash() const
{
    return m_md5hash;
}

void Packet::setMd5hash(QVariant value)
{
    m_md5hash = value.toString();
}

QVariant Packet::dateTime() const
{
    return m_dateTime;
}

void Packet::setDateTime(QVariant value)
{
    m_dateTime = value.toDateTime();
}

QVariant Packet::idBegin() const
{
    return m_idbegin;
}

void Packet::setIdBegin(QVariant value)
{
    m_idbegin = value.toLongLong();
}

QVariant Packet::itemCount() const
{
    return m_itemCount;
}

void Packet::setItemCount(QVariant value)
{
    m_itemCount = value.toLongLong();
}

QVariant Packet::jsonData() const
{
    return m_jsondata;
}

void Packet::setJsonData(QVariant value)
{
    m_jsondata = value.toByteArray();
}

QVariant Packet::status() const
{
    return (qint32)m_status;
}

void Packet::setStatus(QVariant value)
{
    m_status = (Status)value.toInt();
}
