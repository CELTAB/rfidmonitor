#ifndef PACKETDAO_H
#define PACKETDAO_H

#include <QList>
#include <QString>
#include <QSqlDatabase>

#include <core/genericdao.h>

class Packet;

class PacketDAO : public GenericDAO<Packet>
{
    Q_OBJECT
public:
    explicit PacketDAO(QObject *parent = 0);

    static PacketDAO * instance();

    bool insertObject(Packet *packet);
    bool updateObject(Packet *obj);
    bool deleteObject(Packet *packet);

    Packet * getById(qlonglong id, QObject *parent=0);

    QList<Packet *> getAll(QObject *parent=0);

    QList<Packet *> getByMatch(const QString &columnName, QVariant value, QObject *parent=0);

    bool insertObjectList(const QList<Packet *> &list);
    bool updateObjectList(const QList<Packet *> &list);
    bool deleteObjectList(const QList<Packet *> &list);

private:
    QSqlDatabase m_db;
};

#endif // PACKETDAO_H
