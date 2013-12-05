// ****************************************************
// * Codigo Generado por DAOBuilder                   *
// * Fecha: 30/07/2013 10:33:09                       *
// * Luis Valdes 2012 - Todos los Derechos Reservados *
// ****************************************************
#include "rfiddata.h"

Rfiddata::Rfiddata(QObject *parent) :
    QObject(parent)
{

}

Rfiddata::Rfiddata(const QSqlRecord &record, QObject *parent) :
    QObject(parent)
{
    // id, idantena, idpontocoleta, applicationcode, identificationcode, datetime, synced
	setId(record.value(0));
	setIdantena(record.value(1));
    setIdpontocoleta(record.value(2));
    setApplicationcode(record.value(3));
    setIdentificationcode(record.value(4));
    setDatetime(record.value(5));
    setSync(record.value(6));
}

QVariant Rfiddata::id() const
{
    return m_id;
}

void Rfiddata::setId(QVariant value)
{
    m_id = value.toLongLong();
}

// Getters y Setters Definitions
QVariant Rfiddata::idantena() const
{
	return m_idantena;
}

void Rfiddata::setIdantena(QVariant value)
{
    m_idantena = value.toLongLong();
}

QVariant Rfiddata::idpontocoleta() const
{
    return m_idpontocoleta;
}

void Rfiddata::setIdpontocoleta(QVariant value)
{
    m_idpontocoleta = value.toLongLong();
}

QVariant Rfiddata::applicationcode() const
{
	return m_applicationcode;
}

void Rfiddata::setApplicationcode(QVariant value)
{
    m_applicationcode = value.toInt();
}

QVariant Rfiddata::identificationcode() const
{
	return m_identificationcode;
}

void Rfiddata::setIdentificationcode(QVariant value)
{
	m_identificationcode = value.toLongLong();
}

QVariant Rfiddata::datetime() const
{
	return m_datetime;
}

void Rfiddata::setDatetime(QVariant value)
{
    m_datetime = value.toDateTime();
}

QVariant Rfiddata::sync() const
{
    return m_sync;
}

void Rfiddata::setSync(QVariant value)
{
    m_sync = (SyncState) value.toInt();
}
