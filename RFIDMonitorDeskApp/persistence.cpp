#include <QCoreApplication>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

#include "persistence.h"

Persistence *Persistence::instance()
{
    static Persistence *singleton = 0;
    if(!singleton)
        singleton = new Persistence;
    return singleton;
}

bool Persistence::persistConfigurationSent(const QString &mac, const QString &ip, const QString &collectPointName, const QByteArray &json)
{
    // Start new transaction.
    systemConnection()->transaction();
    // Get the new available sequence from database to the new object.
    //        qlonglong id = Functions::getSequence("seq_rfiddata", db);
    //        rfiddata->setId(id);

    //Create the query.
    QSqlQuery query(*systemConnection());
    //        query.prepare("insert into rfiddata (id, idantena, idpontocoleta, applicationcode, identificationcode, datetime, sync) "
    //                      " values(:id, :idantena, :idpontocoleta, :applicationcode, :identificationcode, :datetime, :sync) ");
    //        query.bindValue(":id", rfiddata->id());
    //        query.bindValue(":idantena", rfiddata->idantena());
    //        query.bindValue(":idpontocoleta", rfiddata->idpontocoleta());
    //        query.bindValue(":applicationcode", rfiddata->applicationcode());
    //        query.bindValue(":identificationcode", rfiddata->identificationcode());
    //        query.bindValue(":datetime", rfiddata->datetime());
    //        query.bindValue(":sync", rfiddata->sync());

    // Execute the query.
    query.exec();

    if(query.lastError().type() == QSqlError::NoError){
        // Commit and terminate the transaction.
        systemConnection()->commit();
        return true;
    }else{
        systemConnection()->rollback();
        qDebug() << "SQLError: " << query.lastError().text();
        return false;
    }
}

QSqlDatabase *Persistence::systemConnection()
{
    if(!QSqlDatabase::contains("RFIDMonitorDeskapp"))
    {
        //Path to the database file.
        QString appDirPath(QCoreApplication::applicationDirPath());
        QString sysdbPath(appDirPath + "/sysdb_rfidmonitordeskapp.db");

        //Database type and connection name.
        m_systemConnection = QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "RFIDMonitorDeskapp"));
        m_systemConnection.setDatabaseName(sysdbPath);

        //Try to open the database connection.
        m_systemConnection.open();

        if(m_systemConnection.isOpen()){

            //If connection is open and the tables does not exists, means it's a new/empty database. So create it.

            //            QString createtable = QString(QString("CREATE  TABLE IF NOT EXISTS `rfiddata` (\n") +
            //                                          QString("  `id` INT(16) NOT NULL,\n") +
            //                                          QString("  `idantena` int(16) NOT NULL,\n") +
            //                                          QString("  `idpontocoleta` int(16) NOT NULL,\n") +
            //                                          QString("  `applicationcode` int(16) NOT NULL,\n") +
            //                                          QString("  `identificationcode` int(16) NOT NULL,\n") +
            //                                          QString("  `datetime` datetime NOT NULL,\n") +
            //                                          QString("  `sync` int(2) NOT NULL,\n") +
            //                                          QString("  PRIMARY KEY (`id`) );\n"));
            //            QString createSeq = QString(QString("CREATE  TABLE IF NOT EXISTS `sequences` (\n") +
            //                                        QString("  `name` VARCHAR(200) NOT NULL ,\n") +
            //                                        QString("  `initialvalue` BIGINT(12) NOT NULL ,\n") +
            //                                        QString("  `nextvalue` BIGINT(12) NOT NULL ,\n") +
            //                                        QString("  UNIQUE (`name`) ,\n") +
            //                                        QString("  PRIMARY KEY (`name`) );\n"));
            QSqlQuery query(m_systemConnection);

            // Execute the queries.
//            query.exec(createtable);
//            query.exec(createSeq);
        }
    }

    if(m_systemConnection.lastError().isValid()){
        //Prints the last error occurred with the connection, if exists.
        qDebug() << m_systemConnection.lastError().text();
    }

    return &m_systemConnection;
}

Persistence::Persistence(QObject *parent) :
    QObject(parent)
{
}
