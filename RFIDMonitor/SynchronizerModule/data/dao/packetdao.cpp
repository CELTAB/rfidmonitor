#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSharedPointer>

#include <logger.h>

#include <rfidmonitor.h>

#include <core/sql/sqlquery.h>
#include <core/functions.h>
#include <core/connectionpool.h>

#include "packetdao.h"
#include "../object/packet.h"



PacketDAO::PacketDAO(QObject *parent) :
    GenericDAO<Packet>(parent)
{
    setObjectName("PacketDAO");

    QString appDirPath(QCoreApplication::applicationDirPath());
    QString sysdbPath(appDirPath + "/sysdb.db");
    m_db = QSqlDatabase::addDatabase("QSQLITE", "Packager");
    m_db.setDatabaseName(sysdbPath);

    m_db.open();
}

PacketDAO *PacketDAO::instance()
{
    static QSharedPointer<PacketDAO> singleton(0);
    if(!singleton.data()){
        singleton = QSharedPointer<PacketDAO>(new PacketDAO, &PacketDAO::deleteLater);
        singleton->setParent(0);
    }
    return singleton.data();
}

/*!
 * \brief PacketDAO::insertObject Offers the way to persist 1 Packet object.
 * \param Packet is the object to be inserted.
 * \return true if successfully inserted, false otherwise.
 */
bool PacketDAO::insertObject(Packet *packet)
{
    // Start new transaction.
    m_db.transaction();

    try{
        //Create the query.
        SqlQuery query(&m_db);
        query.prepare("insert into packet (md5hash, datetime, idbegin, item_count, jsondata, status) "
                      " values(:md5hash, :datetime, :idbegin, :item_count, :jsondata, :status) ");
        query.bindValue(":md5hash", packet->md5hash());
        query.bindValue(":datetime", packet->dateTime());
        query.bindValue(":idbegin", packet->idBegin());
        query.bindValue(":item_count", packet->itemCount());
        query.bindValue(":jsondata", packet->jsonData());
        query.bindValue(":status", packet->status());

        // Execute the query.
        query.exec();

        // Commit and terminate the transaction.
        m_db.commit();
        return true;

    }catch(SqlException &ex){
        Logger::instance()->writeRecord(Logger::severity_level::critical, "SynchronizationModule", Q_FUNC_INFO, QString("Transaction Error: %1").arg(ex.errorText()));
        //If is there any exception caught, do rollback and close the transaction, aborting the insertion.
        m_db.rollback();
        return false;
    }
}

bool PacketDAO::insertObjectList(const QList<Packet *> &list)
{
    // Start new transaction.
    m_db.transaction();

    try{
        foreach (Packet *packet, list) {
            //Create the query.
            SqlQuery query(&m_db);
            query.prepare("insert into packet (md5hash, datetime, idbegin, item_count, jsondata, status) "
                          " values(:md5hash, :datetime, :idbegin, :item_count, :jsondata, :status) ");
            query.bindValue(":md5hash", packet->md5hash());
            query.bindValue(":datetime", packet->dateTime());
            query.bindValue(":idbegin", packet->idBegin());
            query.bindValue(":item_count", packet->itemCount());
            query.bindValue(":jsondata", packet->jsonData());
            query.bindValue(":status", packet->status());

            // Execute the query.
            query.exec();
        }

        // Commit and terminate the transaction.
        m_db.commit();
        return true;

    }catch(SqlException &ex){
        Logger::instance()->writeRecord(Logger::severity_level::critical, "SynchronizationModule", Q_FUNC_INFO, QString("Transaction Error: %1").arg(ex.errorText()));
        //If is there any exception caught, do rollback and close the transaction, aborting the insertion.
        m_db.rollback();
        return false;
    }
}

/*!
 * \brief PacketDAO::updateObject Offers the way to update one Packet object.
 * \param Packet is the object to be updated.
 * \return true if successfully updated, false otherwise.
 */
bool PacketDAO::updateObject(Packet *packet)
{
    // Start new transaction.
    m_db.transaction();

    try{
        SqlQuery query(&m_db);
        query.prepare("update packet set idbegin = :idbegin, item_count = :item_count, jsondata = :jsondata, status = :status, "
                      "datetime = :datetime where md5hash = :md5hash ");
        query.bindValue(":md5hash", packet->md5hash());
        query.bindValue(":datetime", packet->dateTime());
        query.bindValue(":idbegin", packet->idBegin());
        query.bindValue(":item_count", packet->itemCount());
        query.bindValue(":jsondata", packet->jsonData());
        query.bindValue(":status", packet->status());
        query.exec();

        // Commit and terminate the transaction.
        m_db.commit();
        return true;

    }catch(SqlException &ex){
        Logger::instance()->writeRecord(Logger::severity_level::critical, "SynchronizationModule", Q_FUNC_INFO, QString("Transaction Error: %1").arg(ex.errorText()));
        //If is there any exception caught, do rollback and close the transaction, aborting the insertion.
        m_db.rollback();
        return false;
    }
}

/*!
 * \brief PacketDAO::updateObjectList Offers the way to update the whole list of Packet objects.
 * \param list is the list of Packet objects to be updated.
 * \return true if successfully updated the whole list, false otherwise.
 */
bool PacketDAO::updateObjectList(const QList<Packet *> &list)
{
    // Start new transaction.
    m_db.transaction();
    try{
        // Loop for all objects in the list.
        foreach (Packet *packet, list) {

            SqlQuery query(&m_db);
            query.prepare("update packet set idbegin = :idbegin, item_count = :item_count, jsondata = :jsondata, status = :status, "
                          "datetime = :datetime where md5hash = :md5hash ");
            query.bindValue(":md5hash", packet->md5hash());
            query.bindValue(":datetime", packet->dateTime());
            query.bindValue(":idbegin", packet->idBegin());
            query.bindValue(":item_count", packet->itemCount());
            query.bindValue(":jsondata", packet->jsonData());
            query.bindValue(":status", packet->status());
            query.exec();
        }

        // Commit and terminate the transaction with the update of all objects inside.
        m_db.commit();
        return true;

    }catch(SqlException &ex){
        Logger::instance()->writeRecord(Logger::severity_level::critical, "SynchronizationModule", Q_FUNC_INFO, QString("Transaction Error: %1").arg(ex.errorText()));
        //If is there any exception caught, do rollback and close the transaction, aborting the insertion.
        m_db.rollback();
        return false;
    }
}

/*!
 * \brief PacketDAO::deleteObject Offers the way to remove one object from database.
 * \param Packet is the object to be deleted from database.
 * \return true if successfully deleted, false otherwise.
 */
bool PacketDAO::deleteObject(Packet *packet)
{
    // Start new transaction.
    m_db.transaction();

    try{
        SqlQuery query(&m_db);
        query.prepare("delete from packet where md5hash = :md5hash");
        query.bindValue(":md5hash", packet->md5hash());
        query.exec();

        // Commit and terminate the transaction.
        m_db.commit();
        return true;

    }catch(SqlException &ex){
        Logger::instance()->writeRecord(Logger::severity_level::critical, "SynchronizationModule", Q_FUNC_INFO, QString("Transaction Error: %1").arg(ex.errorText()));
        //If is there any exception caught, do rollback and close the transaction, aborting the insertion.
        m_db.rollback();
        return false;
    }
}

/*!
 * \brief PacketDAO::deleteObjectList Offers the way to delete the whole list of Packet objects.
 * \param list is the list of Packet objects to be deleted.
 * \return true if successfully deleted the whole list, false otherwise.
 */
bool PacketDAO::deleteObjectList(const QList<Packet *> &list)
{
    // Start new transaction.
    m_db.transaction();

    try{
        foreach (Packet * packet, list) {
            SqlQuery query(&m_db);
            query.prepare("delete from packet where md5hash = :md5hash");
            query.bindValue(":md5hash", packet->md5hash());
            query.exec();
        }

        // Commit and terminate the transaction.
        m_db.commit();
        return true;

    }catch(SqlException &ex){
        Logger::instance()->writeRecord(Logger::severity_level::critical, "SynchronizationModule", Q_FUNC_INFO, QString("Transaction Error: %1").arg(ex.errorText()));
        //If is there any exception caught, do rollback and close the transaction, aborting the insertion.
        m_db.rollback();
        return false;
    }
}

/*!
 * \brief PacketDAO::getById Offers the way to get one object from database by it's id.
 * \param id is the database id of the object.
 * \param parent is the parent object of Packet will be returned.
 * \return Packet * to the object found, or 0 if any exception caugth.
 */
Packet * PacketDAO::getById(qlonglong id, QObject *parent)
{
    try{
        SqlQuery query(&m_db);
        query.prepare("select md5hash, datetime, idbegin, item_count, jsondata, status from packet  where md5hash = :md5hash ");
        query.bindValue(":md5hash", id);
        query.exec();
        Packet *packet = 0;
        if(query.next()){
            /* If the parent is not set to the Packet object, it will be destroyed when this "getById" is done, causing
             *a segmentation fault when try to use the returned pointer.
             * Is necessary to give the Packet object to another class, so it must survive long as his parent.
             */
            packet = new Packet(query.record(), PARENT(parent));
        }
        return packet;
    }catch(SqlException &ex){
        Logger::instance()->writeRecord(Logger::severity_level::error, "SynchronizationModule", Q_FUNC_INFO, QString("Transaction Error: %1").arg(ex.errorText()));
        return 0;
    }
}

/*!
 * \brief PacketDAO::getAll Offers the way to get all registers from database into a list of rfidata objects.
 * \param parent is the parent object of whole list will be returned.
 * \return QList<Packet *> containing the objects found.
 */
QList<Packet *> PacketDAO::getAll(QObject *parent)
{
    QList<Packet *> list;

    try{
        SqlQuery query(&m_db);
        query.prepare("select md5hash, datetime, idbegin, item_count, jsondata, status from packet ");
        query.exec();
        while(query.next()){
            /* If the parent is not set to the Packet object, it will be destroyed when this "getById" is done, causing
             *a segmentation fault when try to use the returned pointer.
             * Is necessary to give the Packet object to another class, so it must survive long as his parent.
             */
            list.append(new Packet(query.record(), parent));
        }
        return list;

    }catch(SqlException &ex){
        Logger::instance()->writeRecord(Logger::severity_level::critical, "SynchronizationModule", Q_FUNC_INFO, QString("Transaction Error: %1").arg(ex.errorText()));
        return list;
    }
}

/*!
 * \brief PacketDAO::getByMatch Offers the way select a list of Packet objects that matches the specified parameter.
 * \param ColumnObject refers to the name of some atribute from packet object.
 * \param value refers to the value of the \a ColumnObject will be restricted.
 * \param parent is the parent of whole returned list.
 * \return QList<Packet *> list of objects found by the restrictive query.
 */
QList<Packet *> PacketDAO::getByMatch(const QString &ColumnObject, QVariant value, QObject *parent)
{
    QList<Packet *> list;

    try{
        SqlQuery query(&m_db);
        /* Creates the "where" restriction with the column name from "ColumnObject" parameter.
         * and the value of it with "value".
         */
        QString sqlQuery = QString("select md5hash, datetime, idbegin, item_count, jsondata, status from packet  where %1 = :value ").arg(ColumnObject);
        query.prepare(sqlQuery);
        query.bindValue(":value", value);
        query.exec();
        while(query.next()){
            /* If the parent is not set to the Packet object, it will be destroyed when this "getById" is done, causing
             *a segmentation fault when try to use the returned pointer.
             * Is necessary to give the Packet object to another class, so it must survive long as his parent.
             */
            list.append(new Packet(query.record(), parent));
        }
        return list;

    }catch(SqlException &ex){
        Logger::instance()->writeRecord(Logger::severity_level::critical, "SynchronizationModule", Q_FUNC_INFO, QString("Transaction Error: %1").arg(ex.errorText()));
        return list;
    }
}
