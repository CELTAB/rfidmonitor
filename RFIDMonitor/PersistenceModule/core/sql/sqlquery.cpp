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

#include <QSqlRecord>
#include <QSqlError>
#include <QSqlResult>
#include <QVariant>
#include <QDebug>

#include "sqlquery.h"

#include "exception/sqlconnectionexception.h"
#include "exception/sqlstatementexception.h"
#include "exception/sqltransactionexception.h"
#include "exception/sqlunknownexception.h"


SqlQuery::SqlQuery(const QString &query, QSqlDatabase *db) :
    m_sqlQuery(query, *db)
{
}

SqlQuery::SqlQuery(QSqlDatabase *db) :
    m_sqlQuery(*db)
{
}

void SqlQuery::addBindValue(const QVariant &val, QSql::ParamType paramType)
{
    m_sqlQuery.addBindValue(val, paramType);
}

int SqlQuery::at() const
{
    return m_sqlQuery.at();
}

void SqlQuery::bindValue(const QString &placeholder, const QVariant &val, QSql::ParamType paramType)
{
    m_sqlQuery.bindValue(placeholder, val, paramType);
}

void SqlQuery::bindValue(int pos, const QVariant &val, QSql::ParamType paramType)
{
    m_sqlQuery.bindValue(pos, val, paramType);
}

QVariant SqlQuery::boundValue(const QString &placeholder) const
{
    return m_sqlQuery.boundValue(placeholder);
}

QVariant SqlQuery::boundValue(int pos) const
{
    return m_sqlQuery.boundValue(pos);
}

QMap<QString, QVariant> SqlQuery::boundValues() const
{
    return m_sqlQuery.boundValues();
}

void SqlQuery::clear()
{
    m_sqlQuery.clear();
}

const QSqlDriver *SqlQuery::driver() const
{
    return m_sqlQuery.driver();
}

/*!
 * \brief SqlQuery::exec Execute a query with capability to throw SqlException
 * if necessary.
 * \param query the sql command.
 * \return true if successfully executed, false otherwise.
 */
bool SqlQuery::exec(const QString &query) throw(SqlException)
{
    bool b = m_sqlQuery.exec(query);
    QSqlError sqlError = m_sqlQuery.lastError();
    switch(sqlError.type ())
    {
    case QSqlError::NoError:
        return b;
    case QSqlError::ConnectionError:
        throw SqlConnectionException(sqlError);
    case QSqlError::StatementError:
        if(sqlError.databaseText() == "MySQL server has gone away"){
            throw SqlConnectionException(sqlError);
        }else{
            throw SqlStatementException(sqlError);
        }
    case QSqlError::TransactionError:
        throw SqlTransactionException(sqlError);
    case QSqlError::UnknownError:
        throw SqlUnknownException(sqlError);
    default:
        throw SqlUnknownException(sqlError);
    }
}

/*!
 * \brief SqlQuery::exec Execute a existing query with capability to throw SqlException
 * if necessary.
 * \return true if successfully executed, false otherwise.
 */
bool SqlQuery::exec() throw(SqlException)
{
    bool b = m_sqlQuery.exec();

    QSqlError sqlError = m_sqlQuery.lastError();
    switch(sqlError.type ())
    {
    case QSqlError::NoError:
        return b;
    case QSqlError::ConnectionError:

    case QSqlError::StatementError:
        if(sqlError.databaseText() == "MySQL server has gone away"){
            throw SqlConnectionException(sqlError);
        }else{
            throw SqlStatementException(sqlError);
        }
    case QSqlError::TransactionError:
        throw SqlTransactionException(sqlError);
    case QSqlError::UnknownError:
        throw SqlUnknownException(sqlError);
    default:
        throw SqlException(sqlError);
    }
}

/*!
 * \brief SqlQuery::execBatch Execute a BatchMode query with capability to throw SqlException
 * if necessary.
 * \param mode refers to the BatchExecutionMode
 * \return true if successfully executed, false otherwise.
 */
bool SqlQuery::execBatch(QSqlQuery::BatchExecutionMode mode) throw(SqlException)
{
    bool b = m_sqlQuery.execBatch(mode);
    QSqlError sqlError = m_sqlQuery.lastError();
    switch(sqlError.type ()){
    case QSqlError::NoError:
        return b;
    case QSqlError::ConnectionError:
        throw SqlConnectionException(sqlError);
    case QSqlError::StatementError:
        if(sqlError.databaseText() == "MySQL server has gone away"){
            throw SqlConnectionException(sqlError);
        }else{
            throw SqlStatementException(sqlError);
        }
    case QSqlError::TransactionError:
        throw SqlTransactionException(sqlError);
    case QSqlError::UnknownError:
        throw SqlUnknownException(sqlError);
    default:
        throw SqlException(sqlError);
    }
}

QString SqlQuery::executedQuery() const
{
    return m_sqlQuery.executedQuery();
}

void SqlQuery::finish()
{
    m_sqlQuery.finish();
}

bool SqlQuery::first()
{
    return m_sqlQuery.first();
}

bool SqlQuery::isActive() const
{
    return m_sqlQuery.isActive();
}

bool SqlQuery::isForwardOnly() const
{
    return m_sqlQuery.isForwardOnly();
}

bool SqlQuery::isNull(int field) const
{
    return m_sqlQuery.isNull(field);
}

bool SqlQuery::isSelect() const
{
    return m_sqlQuery.isSelect();
}

bool SqlQuery::isValid() const
{
    return m_sqlQuery.isValid();
}

bool SqlQuery::last()
{
    return m_sqlQuery.last();
}

QSqlError SqlQuery::lastError() const
{
    return m_sqlQuery.lastError();
}

QVariant SqlQuery::lastInsertId() const
{
    return m_sqlQuery.lastInsertId();
}

QString SqlQuery::lastQuery() const
{
    return m_sqlQuery.lastQuery();
}

bool SqlQuery::next()
{
    return m_sqlQuery.next();
}

bool SqlQuery::nextResult()
{
    return m_sqlQuery.nextResult();
}

int SqlQuery::numRowsAffected() const
{
    return m_sqlQuery.numRowsAffected();
}

QSql::NumericalPrecisionPolicy SqlQuery::numericalPrecisionPolicy() const
{
    return m_sqlQuery.numericalPrecisionPolicy();
}

bool SqlQuery::prepare(const QString &query)
{
    return m_sqlQuery.prepare(query);
}

bool SqlQuery::previous()
{
    return m_sqlQuery.previous();
}

QSqlRecord SqlQuery::record() const
{
    return m_sqlQuery.record();
}

const QSqlResult *SqlQuery::result() const
{
    return m_sqlQuery.result();
}

bool SqlQuery::seek(int index, bool relative)
{
    return m_sqlQuery.seek(index, relative);
}

void SqlQuery::setForwardOnly(bool forward)
{
    m_sqlQuery.setForwardOnly(forward);
}

void SqlQuery::setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy precisionPolicy)
{
    m_sqlQuery.setNumericalPrecisionPolicy(precisionPolicy);
}

int SqlQuery::size() const
{
    return m_sqlQuery.size();
}

QVariant SqlQuery::value(int index) const
{
    return m_sqlQuery.value(index);
}

QSqlQuery &SqlQuery::operator =(const QSqlQuery &other)
{
    m_sqlQuery = other;
    return m_sqlQuery;
}

QSqlQuery *SqlQuery::query()
{
    return &m_sqlQuery;
}
