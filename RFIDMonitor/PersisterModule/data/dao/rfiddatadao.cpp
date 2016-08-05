// ****************************************************
// * Codigo Generado por DAOBuilder                   *
// * Fecha: 30/07/2013 10:33:09                       *
// * Luis Valdes 2012 - Todos los Derechos Reservados *
// ****************************************************
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSharedPointer>
#include <logger.h>

#include <rfidmonitor.h>

#include <core/sql/sqlquery.h>
#include <core/functions.h>
#include <core/connectionpool.h>
#include <systemevents.h>

#include "rfiddatadao.h"
#include "object/rfiddata.h"

RfiddataDAO::RfiddataDAO(QObject *parent) :
    GenericDAO<Rfiddata>(parent)
{
    setObjectName("RfiddataDAO");
    m_module = "PersistenceModule";
}

RfiddataDAO::~RfiddataDAO()
{
    Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("Destructor called."));
    emit SystemEvents::instance()->General(SystemEvents::KLosingData);
}

QString RfiddataDAO::serviceNameInsertObject() const
{
    return "persistence.insert_object";
}

/*!
 * \brief RfiddataDAO::instance Singleton of RfiddataDAO
 * \return return the unique instance of RfiddataDAO.
 */
RfiddataDAO * RfiddataDAO::instance()
{
    static QSharedPointer<RfiddataDAO> singleton(0);
    if(! singleton.data()){
        singleton = QSharedPointer<RfiddataDAO>(new RfiddataDAO, &RfiddataDAO::deleteLater);
        singleton->setParent(0);
    }
    return singleton.data();
}

/*!
 * \brief RfiddataDAO::insertObject Offers the way to persist 1 Rfiddata object.
 * \param rfiddata is the object to be inserted.
 * \return true if successfully inserted, false otherwise.
 */
bool RfiddataDAO::insertObject(Rfiddata *rfiddata)
{
    // Get the connection with the database.
    QSqlDatabase *db = ConnectionPool::instance()->systemConnection();

    // Start new transaction.
    db->transaction();

    try{
        // Get the new available sequence from database to the new object.
        qlonglong id = Functions::getSequence("seq_rfiddata", db);
        rfiddata->setId(id);

        //Create the query.
        SqlQuery query(db);
        query.prepare("insert into rfiddata (id, idantena, idpontocoleta, applicationcode, identificationcode, datetime, sync) "
                      " values(:id, :idantena, :idpontocoleta, :applicationcode, :identificationcode, :datetime, :sync) ");
        query.bindValue(":id", rfiddata->id());
        query.bindValue(":idantena", rfiddata->idantena());
        query.bindValue(":idpontocoleta", rfiddata->idpontocoleta());
        query.bindValue(":applicationcode", rfiddata->applicationcode());
        query.bindValue(":identificationcode", rfiddata->identificationcode());
        query.bindValue(":datetime", rfiddata->datetime());
        query.bindValue(":sync", rfiddata->sync());

        // Execute the query.
        query.exec();

        // Commit and terminate the transaction.
        db->commit();

        Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("RFIDDATA INSERTED. IdentificationCode [%1]").arg(rfiddata->identificationcode().toString()));

        return true;

    }catch(SqlException &ex){
        Logger::instance()->writeRecord(Logger::severity_level::critical, m_module, Q_FUNC_INFO, QString("Transaction Error: %1").arg(ex.errorText()));
        //If is there any exception caught, do rollback and close the transaction, aborting the insertion.
        db->rollback();
        emit SystemEvents::instance()->General(SystemEvents::KLosingData);
        return false;
    }
}

bool RfiddataDAO::insertObjectList(const QList<Rfiddata *> &list)
{
    // Get the connection with the database.
    QSqlDatabase *db = ConnectionPool::instance()->systemConnection();

    // Start new transaction.
    db->transaction();

    try{
        QString rfidDebugList;
        foreach (Rfiddata *rfiddata, list) {
            // Get the new available sequence from database to the new object.
            qlonglong id = Functions::getSequence("seq_rfiddata", db);
            rfiddata->setId(id);

            //Create the query.
            SqlQuery query(db);
            query.prepare("insert into rfiddata (id, idantena, idpontocoleta, applicationcode, identificationcode, datetime, sync) "
                          " values(:id, :idantena, :idpontocoleta, :applicationcode, :identificationcode, :datetime, :sync) ");
            query.bindValue(":id", rfiddata->id());
            query.bindValue(":idantena", rfiddata->idantena());
            query.bindValue(":idpontocoleta", rfiddata->idpontocoleta());
            query.bindValue(":applicationcode", rfiddata->applicationcode());
            query.bindValue(":identificationcode", rfiddata->identificationcode());
            query.bindValue(":datetime", rfiddata->datetime());
            query.bindValue(":sync", rfiddata->sync());

            // Execute the query.
            query.exec();

            rfidDebugList.append(rfiddata->identificationcode().toString() + " , ");
        }

        // Commit and terminate the transaction.
        db->commit();

        Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("RFIDDATA INSERTED. IdentificationCode [%1]").arg(rfidDebugList));

        return true;

    }catch(SqlException &ex){
        Logger::instance()->writeRecord(Logger::severity_level::critical, m_module, Q_FUNC_INFO, QString("Transaction Error: %1").arg(ex.errorText()));
        //If is there any exception caught, do rollback and close the transaction, aborting the insertion.
        db->rollback();
        emit SystemEvents::instance()->General(SystemEvents::KLosingData);
        return false;
    }
}

/*!
 * \brief RfiddataDAO::updateObject Offers the way to update one Rfiddata object.
 * \param rfiddata is the object to be updated.
 * \return true if successfully updated, false otherwise.
 */
bool RfiddataDAO::updateObject(Rfiddata *rfiddata)
{
    // Get the connection with the database.
    QSqlDatabase *db = ConnectionPool::instance()->systemConnection();

    // Check if the Rfiddata object have the id. If haven't there is no way to update it. LUIS Se est´a bem aqui, tem que colocar no updateList tmb.
    if(rfiddata->id().isNull()){
        Logger::instance()->writeRecord(Logger::severity_level::critical, m_module, Q_FUNC_INFO, QString("Object Without ID"));
        return false;
    }

    // Start new transaction.
    db->transaction();

    try{
        SqlQuery query(db);
        query.prepare("update rfiddata set idantena = :idantena, idpontocoleta = :idpontocoleta, applicationcode = :applicationcode, identificationcode = :identificationcode, "
                      "datetime = :datetime, sync = :sync where id = :id ");
        query.bindValue(":id", rfiddata->id());
        query.bindValue(":idantena", rfiddata->idantena());
        query.bindValue(":idpontocoleta", rfiddata->idpontocoleta());
        query.bindValue(":applicationcode", rfiddata->applicationcode());
        query.bindValue(":identificationcode", rfiddata->identificationcode());
        query.bindValue(":datetime", rfiddata->datetime());
        query.bindValue(":sync", rfiddata->sync());
        query.exec();

        // Commit and terminate the transaction.
        db->commit();
        return true;

    }catch(SqlException &ex){
        Logger::instance()->writeRecord(Logger::severity_level::critical, m_module, Q_FUNC_INFO, QString("Transaction Error: %1").arg(ex.errorText()));
        //If is there any exception caught, do rollback and close the transaction, aborting the insertion.
        db->rollback();
        emit SystemEvents::instance()->General(SystemEvents::KLosingData);
        return false;
    }
}

/*!
 * \brief RfiddataDAO::updateObjectList Offers the way to update the whole list of Rfiddata objects.
 * \param list is the list of Rfiddata objects to be updated.
 * \return true if successfully updated the whole list, false otherwise.
 */
bool RfiddataDAO::updateObjectList(const QList<Rfiddata *> &list)
{
    // Get the connection with the database.
    QSqlDatabase *db = ConnectionPool::instance()->systemConnection();

    // Start new transaction.
    db->transaction();
    try{
        // Loop for all objects in the list.
        foreach (Rfiddata *rfiddata, list) {

            SqlQuery query(db);
            query.prepare("update rfiddata set idantena = :idantena, idpontocoleta = :idpontocoleta, applicationcode = :applicationcode, identificationcode = :identificationcode, "
                          "datetime = :datetime, sync = :sync where id = :id ");
            query.bindValue(":id", rfiddata->id());
            query.bindValue(":idantena", rfiddata->idantena());
            query.bindValue(":idpontocoleta", rfiddata->idpontocoleta());
            query.bindValue(":applicationcode", rfiddata->applicationcode());
            query.bindValue(":identificationcode", rfiddata->identificationcode());
            query.bindValue(":datetime", rfiddata->datetime());
            query.bindValue(":sync", rfiddata->sync());
            query.exec();
        }

        // Commit and terminate the transaction with the update of all objects inside.
        db->commit();
        return true;

    }catch(SqlException &ex){
        Logger::instance()->writeRecord(Logger::severity_level::critical, m_module, Q_FUNC_INFO, QString("Transaction Error: %1").arg(ex.errorText()));
        //If is there any exception caught, do rollback and close the transaction, aborting the insertion.
        db->rollback();
        emit SystemEvents::instance()->General(SystemEvents::KLosingData);
        return false;
    }
}

/*!
 * \brief RfiddataDAO::deleteObject Offers the way to remove one object from database.
 * \param rfiddata is the object to be deleted from database.
 * \return true if successfully deleted, false otherwise.
 */
bool RfiddataDAO::deleteObject(Rfiddata *rfiddata)
{
    // Get the connection with the database.
    QSqlDatabase *db = ConnectionPool::instance()->systemConnection();

    // Start new transaction.
    db->transaction();

    try{
        SqlQuery query(db);
        query.prepare("delete from rfiddata where id = :id");
        query.bindValue(":id", rfiddata->id());
        query.exec();

        // Commit and terminate the transaction.
        db->commit();
        return true;

    }catch(SqlException &ex){
        Logger::instance()->writeRecord(Logger::severity_level::critical, m_module, Q_FUNC_INFO, QString("Transaction Error: %1").arg(ex.errorText()));
        //If is there any exception caught, do rollback and close the transaction, aborting the insertion.
        db->rollback();
        emit SystemEvents::instance()->General(SystemEvents::KSoftProblem);
        return false;
    }
}

/*!
 * \brief RfiddataDAO::deleteObjectList Offers the way to delete the whole list of Rfiddata objects.
 * \param list is the list of Rfiddata objects to be deleted.
 * \return true if successfully deleted the whole list, false otherwise.
 */
bool RfiddataDAO::deleteObjectList(const QList<Rfiddata *> &list)
{
    // Get the connection with the database.
    QSqlDatabase *db = ConnectionPool::instance()->systemConnection();

    // Start new transaction.
    db->transaction();

    try{
        foreach (Rfiddata * rfiddata, list) {
            SqlQuery query(db);
            query.prepare("delete from rfiddata where id = :id");
            query.bindValue(":id", rfiddata->id());
            query.exec();            
        }

        // Commit and terminate the transaction.
        db->commit();
        return true;

    }catch(SqlException &ex){
        Logger::instance()->writeRecord(Logger::severity_level::critical, m_module, Q_FUNC_INFO, QString("Transaction Error: %1").arg(ex.errorText()));
        //If is there any exception caught, do rollback and close the transaction, aborting the insertion.
        db->rollback();
        emit SystemEvents::instance()->General(SystemEvents::KSoftProblem);
        return false;
    }
}

/*!
 * \brief RfiddataDAO::getById Offers the way to get one object from database by it's id.
 * \param id is the database id of the object.
 * \param parent is the parent object of Rfiddata will be returned.
 * \return Rfiddata * to the object found, or 0 if any exception caugth.
 */
Rfiddata * RfiddataDAO::getById(qlonglong id, QObject *parent)
{
    // Get the connection with the database.
    QSqlDatabase *db = ConnectionPool::instance()->systemConnection();

    try{
        SqlQuery query(db);
        query.prepare("select id, idantena, idpontocoleta, applicationcode, identificationcode, datetime sync from rfiddata  where id = :id ");
        query.bindValue(":id", id);
        query.exec();
        Rfiddata *rfiddata = 0;
        if(query.next()){
            /* If the parent is not set to the rfiddata object, it will be destroyed when this "getById" is done, causing
             *a segmentation fault when try to use the returned pointer.
             * Is necessary to give the rfiddata object to another class, so it must survive long as his parent.
             */
            rfiddata = new Rfiddata(query.record(), PARENT(parent));
        }
        return rfiddata;
    }catch(SqlException &ex){
        Logger::instance()->writeRecord(Logger::severity_level::error, m_module, Q_FUNC_INFO, QString("Transaction Error: %1").arg(ex.errorText()));
        emit SystemEvents::instance()->General(SystemEvents::KSoftProblem);
        return 0;
    }
}

/*!
 * \brief RfiddataDAO::getAll Offers the way to get all registers from database into a list of rfidata objects.
 * \param parent is the parent object of whole list will be returned.
 * \return QList<Rfiddata *> containing the objects found.
 */
QList<Rfiddata *> RfiddataDAO::getAll(QObject *parent)
{
    QList<Rfiddata *> list;

    // Get the connection with the database.
    QSqlDatabase *db = ConnectionPool::instance()->systemConnection();

    try{
        SqlQuery query(db);
        query.prepare("select id, idantena, idpontocoleta, applicationcode, identificationcode, datetime sync from rfiddata ");
        query.exec();
        while(query.next()){
            /* If the parent is not set to the rfiddata object, it will be destroyed when this "getById" is done, causing
             *a segmentation fault when try to use the returned pointer.
             * Is necessary to give the rfiddata object to another class, so it must survive long as his parent.
             */
            list.append(new Rfiddata(query.record(), parent));
        }
        return list;

    }catch(SqlException &ex){
        Logger::instance()->writeRecord(Logger::severity_level::critical, m_module, Q_FUNC_INFO, QString("Transaction Error: %1").arg(ex.errorText()));
        emit SystemEvents::instance()->General(SystemEvents::KSoftProblem);
        return list;
    }
}

/*!
 * \brief RfiddataDAO::getByMatch Offers the way select a list of Rfiddata objects that matches the specified parameter.
 * \param ColumnObject refers to the name of some atribute from Rfiddata object.
 * \param value refers to the value of the \a ColumnObject will be restricted.
 * \param parent is the parent of whole returned list.
 * \return QList<Rfiddata *> list of objects found by the restrictive query.
 */
QList<Rfiddata *> RfiddataDAO::getByMatch(const QString &ColumnObject, QVariant value, QObject *parent)
{
    QList<Rfiddata *> list;

    // Get the connection with the database.
    QSqlDatabase *db = ConnectionPool::instance()->systemConnection();

    try{
        SqlQuery query(db);
        /* Creates the "where" restriction with the column name from "ColumnObject" parameter.
         * and the value of it with "value".
         */
        QString sqlQuery = QString("select id, idantena, idpontocoleta, applicationcode, identificationcode, datetime sync from rfiddata  where %1 = :value ").arg(ColumnObject);
        query.prepare(sqlQuery);
        query.bindValue(":value", value);
        query.exec();
        while(query.next()){
            /* If the parent is not set to the rfiddata object, it will be destroyed when this "getById" is done, causing
             *a segmentation fault when try to use the returned pointer.
             * Is necessary to give the rfiddata object to another class, so it must survive long as his parent.
             */
            list.append(new Rfiddata(query.record(), parent));
        }
        return list;

    }catch(SqlException &ex){
        Logger::instance()->writeRecord(Logger::severity_level::critical, m_module, Q_FUNC_INFO, QString("Transaction Error: %1").arg(ex.errorText()));
        emit SystemEvents::instance()->General(SystemEvents::KSoftProblem);
        return list;
    }
}
