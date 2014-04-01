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

class QSqlRecord;

/*!
 * \brief The Rfiddata class is an ORM represeting the table "rfiddata" of a database where all read data is stored.
 */
class  Rfiddata : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QVariant idantena
		READ idantena
		WRITE setIdantena)
    Q_PROPERTY(QVariant idpontocoleta
        READ idpontocoleta
        WRITE setIdpontocoleta)
	Q_PROPERTY(QVariant applicationcode
		READ applicationcode
		WRITE setApplicationcode)
	Q_PROPERTY(QVariant identificationcode
		READ identificationcode
		WRITE setIdentificationcode)
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
	QVariant idantena() const;
	void setIdantena(QVariant);
    QVariant idpontocoleta() const;
    void setIdpontocoleta(QVariant);
	QVariant applicationcode() const;
	void setApplicationcode(QVariant);
	QVariant identificationcode() const;
	void setIdentificationcode(QVariant);
	QVariant datetime() const;
	void setDatetime(QVariant);
    QVariant sync() const;
    void setSync(QVariant);

private:
    qlonglong m_id;
	qlonglong m_idantena;
    qlonglong m_idpontocoleta;
    qlonglong m_applicationcode;
	qlonglong m_identificationcode;
    QDateTime m_datetime;
    SyncState m_sync;

};

#endif // RFIDDATA_H
