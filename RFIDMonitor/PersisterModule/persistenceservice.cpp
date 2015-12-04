#include <QMutexLocker>

#include "persistenceservice.h"
#include "data/dao/rfiddatadao.h"
#include "core/connectionpool.h"
#include "logger.h"

PersistenceService::PersistenceService(QObject *parent) :
    PersistenceInterface(parent)
{
    /*
     * The function ConnectionPool::instance() create for the first time
     * the unique instance of the object class here, to preserve the life
     * cycle of the object together with the PersistenceService.
     */
    ConnectionPool::instance();
}

QString PersistenceService::serviceName() const
{
    return "persistence.service";
}

void PersistenceService::init()
{

}

ServiceType PersistenceService::type()
{
    return ServiceType::KPersister;
}

QList<Rfiddata *> PersistenceService::getObjectList(const QString &ColumnObject, QVariant value, QObject *parent)
{
    QMutexLocker locker(&m_mutex);

    return RfiddataDAO::instance()->getByMatch(ColumnObject, value, parent);
}

void PersistenceService::insertObjectList(const QList<Rfiddata *> &data)
{
    QMutexLocker locker(&m_mutex);

    RfiddataDAO::instance()->insertObjectList(data);
}

void PersistenceService::updateObjectList(const QList<Rfiddata *> &data)
{
    QMutexLocker locker(&m_mutex);

    RfiddataDAO::instance()->updateObjectList(data);
}

void PersistenceService::deleteObjectList(const QList<Rfiddata *> &data)
{
    QMutexLocker locker(&m_mutex);

    RfiddataDAO::instance()->deleteObjectList(data);
}
