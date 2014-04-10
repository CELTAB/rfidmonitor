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

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDateTime>
#include <logger.h>

#include "functions.h"
#include "sql/sqlquery.h"
#include "sql/exception/sqlexception.h"

/*!
 * \brief Functions::getSequence returns the next available sequence of specified table from database.
 * \param className refers to the class name, which is the table name in database.
 * \param db refers to the database connection.
 * \return next available sequence of specified table.
 */
qlonglong Functions::getSequence(const QString &className, QSqlDatabase *db)
{
    qlonglong result = -1;
    try {
        SqlQuery query(db);
        query.prepare("select nextvalue from sequences where name = :name");
        query.bindValue(":name", className);
        query.exec();
        if(query.next()){
            // If sequence exists, update the table with the new available sequence.
            result = query.value(0).toLongLong();
            SqlQuery updateQuery(db);
            updateQuery.prepare("update sequences set nextvalue = :value where name = :name");
            qlonglong nextValue = result + 1;
            updateQuery.bindValue(":value", nextValue);
            updateQuery.bindValue(":name", className);
            updateQuery.exec();
        }else{
            // If sequence does not exists, the system creates one and returns its next value
            SqlQuery query(db);
            query.prepare("insert into sequences(name, initialvalue, nextvalue)"
                          "values(:name, :initialvalue, :nextvalue)");
            query.bindValue(":name", className);
            query.bindValue(":initialvalue", 1);
            query.bindValue(":nextvalue", 1);
            query.exec();
            return getSequence(className, db);
        }
    }catch(SqlException &ex){
        Logger::instance()->writeRecord(Logger::severity_level::critical, "PersistenceModule", Q_FUNC_INFO, QString("Transaction Error: %1").arg(ex.what()));
        return -1;
    }
    return result;
}

/*!
 * \brief Functions::getSystemDate get the system date.
 * \return QDateTime system date.
 */
QDateTime Functions::getSystemDate()
{
    return QDateTime::currentDateTime();
}
