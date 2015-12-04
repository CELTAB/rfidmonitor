/****************************************************************************
**
** http://sourceforge.net/projects/easy-crud/
**
** Copyright (C) 2010, 2011, 2012  Luis Valdes (luisvaldes88@gmail.com)
**
** This file is part of the EZCRUD library
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**
****************************************************************************/

#include <QCoreApplication>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <logger.h>

#include "connectionpool.h"

ConnectionPool::ConnectionPool(QObject *parent) :
    QObject(parent)
{
    m_module = "PersistenceModule";
    //to be possible to find this object with findChild() or findChildren(), a name is set. LUIS
    setObjectName("ConnectionPool");
}

/*!
 * \brief ConnectionPool::instance singleton of ConnectionPool.
 * \return return the unique instance of ConnectionPool.
 */
ConnectionPool *ConnectionPool::instance()
{
    static ConnectionPool *singleton = 0;
    if(!singleton){
        singleton = new ConnectionPool(qApp);
    }
    return singleton;
}

/*!
 * \brief ConnectionPool::systemConnection gets the connection to database, or create it
 *if not exists.
 * \return QSqlDatabase *
 */
QSqlDatabase * ConnectionPool::systemConnection()
{
    if(!QSqlDatabase::contains("System")){
        //Path to the database file.
        QString appDirPath(QCoreApplication::applicationDirPath());
        QString sysdbPath(appDirPath + "/sysdb.db");

        //Database type and connection name.
        m_systemConnection = QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "System"));
        m_systemConnection.setDatabaseName(sysdbPath);

        //Try to open the database connection.
        m_systemConnection.open();

        if(m_systemConnection.isOpen()){

            //If connection is open and the tables does not exists, means it's a new/empty database. So create it.

            QString createtable = QString(QString("CREATE  TABLE IF NOT EXISTS `rfiddata` (\n") +
                                          QString("  `id` INT(16) NOT NULL,\n") +
                                          QString("  `rfidcode` int(16) NOT NULL,\n") +
                                          QString("  `extra_data` TEXT NOT NULL,\n") +
                                          QString("  `datetime` datetime NOT NULL,\n") +
                                          QString("  `sync` int(2) NOT NULL,\n") +
                                          QString("  PRIMARY KEY (`id`) );\n"));
            QString createSeq = QString(QString("CREATE  TABLE IF NOT EXISTS `sequences` (\n") +
                                        QString("  `name` VARCHAR(200) NOT NULL ,\n") +
                                        QString("  `initialvalue` BIGINT(12) NOT NULL ,\n") +
                                        QString("  `nextvalue` BIGINT(12) NOT NULL ,\n") +
                                        QString("  UNIQUE (`name`) ,\n") +
                                        QString("  PRIMARY KEY (`name`) );\n"));
            QString createPackages = QString(QString("CREATE  TABLE IF NOT EXISTS `packet` (\n") +
                                        QString("  `md5hash` VARCHAR(32) NOT NULL ,\n") +
                                        QString("  `datetime` datetime NOT NULL,\n") +
                                        QString("  `idbegin` BIGINT(12) NOT NULL ,\n") +
                                        QString("  `item_count` BIGINT(12) NOT NULL ,\n") +
                                        QString("  `jsondata` BLOB NOT NULL ,\n") +
                                        QString("  `status` int(2) NOT NULL,\n") +
                                        QString("  PRIMARY KEY (`md5hash`) );\n"));
            QSqlQuery query(m_systemConnection);

            // Execute the queries.
            query.exec(createtable);
            query.exec(createSeq);
            query.exec(createPackages);
        }
    }

    if(m_systemConnection.lastError().isValid())
        Logger::instance()->writeRecord(Logger::severity_level::critical, m_module, Q_FUNC_INFO, QString("Error: %1").arg(m_systemConnection.lastError().text()));

    return &m_systemConnection;
}
