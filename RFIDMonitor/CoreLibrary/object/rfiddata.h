// ****************************************************
// * Codigo Generado por DAOBuilder                   *
// * Fecha: 30/07/2013 10:33:09                       *
// * Luis Valdes 2012 - Todos los Derechos Reservados *
// ****************************************************
#ifndef RFIDDATA_H
#define RFIDDATA_H

#include <QObject>
#include <QVariant>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>

class QSqlRecord;

/*!
 * \brief The Rfiddata class is an ORM represeting the table "rfiddata" of a database where all read data is stored.
 */
class  Rfiddata : public QObject
{
	Q_OBJECT
    Q_PROPERTY(QVariant rfidcode
               READ rfidcode
               WRITE setRfidcode)
    Q_PROPERTY(QJsonObject extraData
               READ extraData
               WRITE setExtraData)
	Q_PROPERTY(QVariant datetime
		READ datetime
		WRITE setDatetime)
    Q_PROPERTY(QVariant sync
        READ sync
        WRITE setSync)

public:

    enum SyncState {KNotSynced = 0, KSynced, KPending};

	explicit Rfiddata(QObject *parent = 0);
	explicit Rfiddata(const QSqlRecord &, QObject *parent = 0);

    QVariant id() const;
    void setId(QVariant value);
    QVariant rfidcode() const;
    void setRfidcode(QVariant value);
	QVariant datetime() const;
	void setDatetime(QVariant);
    QJsonObject extraData() const;
    void setExtraData(QJsonObject value);
    QVariant sync() const;
    void setSync(QVariant);

private:
    qlonglong m_id;
    qlonglong m_rfidcode;
    QJsonObject m_extradata;
    QDateTime m_datetime;
    SyncState m_sync;

};

#endif // RFIDDATA_H
