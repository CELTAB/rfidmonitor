// ****************************************************
// * Codigo Generado por DAOBuilder                   *
// * Fecha: 30/07/2013 10:33:09                       *
// * Luis Valdes 2012 - Todos los Derechos Reservados *
// ****************************************************
#ifndef RFIDDATADAO_H
#define RFIDDATADAO_H

#include <QList>
#include <QString>

#include "../../core/genericdao.h"

class Rfiddata;

/*!
 * \brief The RfiddataDAO class is responsible to manipulate the persistence
 * of Rfiddata object in the database.
 */
class  RfiddataDAO : public GenericDAO<Rfiddata>
{
	Q_OBJECT

public:
    RfiddataDAO(QObject *parent = 0);

    QString serviceNameInsertObject() const;

	static RfiddataDAO * instance();

    bool insertObject(Rfiddata *obj);
    bool insertObjectList(const QList<Rfiddata *> &list);
    bool updateObject(Rfiddata *obj);
    bool updateObjectList(const QList<Rfiddata *> &list);
    bool deleteObject(Rfiddata *rfiddata);
    bool deleteObjectList(const QList<Rfiddata *> &list);

    Rfiddata * getById(qlonglong id, QObject *parent=0);

    QList<Rfiddata *> getAll(QObject *parent=0);

    QList<Rfiddata *> getByMatch(const QString &columnName, QVariant value, QObject *parent=0);

private:
    QString m_module;

};

#endif // RFIDDATADAO_H
