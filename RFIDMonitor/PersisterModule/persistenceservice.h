#ifndef PERSISTENCESERVICE_H
#define PERSISTENCESERVICE_H

#include <QObject>
#include <QMutex>

#include <core/interfaces.h>

class PersistenceService : public PersistenceInterface
{
    Q_OBJECT

public:
    explicit PersistenceService(QObject *parent = 0);
    QString serviceName() const;
    void init();
    ServiceType type();
    QList<Rfiddata *> getObjectList(const QString &ColumnObject, QVariant value, QObject *parent);
    void insertObjectList(const QList<Rfiddata *> &data);
    void updateObjectList(const QList<Rfiddata *> &data);
    void deleteObjectList(const QList<Rfiddata *> &data);

private:
    QMutex m_mutex;

};

#endif // PERSISTENCESERVICE_H
