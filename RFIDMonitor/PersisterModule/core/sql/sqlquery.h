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

#ifndef SQLQUERY_H
#define SQLQUERY_H

#include <QSqlQuery>

#include "exception/sqlexception.h"

/*!
 * \brief The SqlQuery class have functions from QSqlQuery overwritten. Giving the
 * possibility to control the behavior of all these functions.
 *
 * One new control introduced is the capability to throw custom SqlException when executing
 * the functions exec() and execBatch().
 */
class SqlQuery
{
public:
    SqlQuery ( const QString & query, QSqlDatabase *db);
    SqlQuery ( QSqlDatabase *db);

    void addBindValue ( const QVariant & val, QSql::ParamType paramType = QSql::In );
    int	at () const;
    void bindValue ( const QString & placeholder, const QVariant & val, QSql::ParamType paramType = QSql::In );
    void bindValue ( int pos, const QVariant & val, QSql::ParamType paramType = QSql::In );
    QVariant boundValue ( const QString & placeholder ) const;
    QVariant boundValue ( int pos ) const;
    QMap<QString, QVariant> boundValues () const;
    void clear ();
    const QSqlDriver * driver () const;

    bool exec ( const QString & query ) throw(SqlException);
    bool exec () throw(SqlException);
    bool execBatch ( QSqlQuery::BatchExecutionMode mode = QSqlQuery::ValuesAsRows )throw(SqlException);

    QString executedQuery () const;
    void finish ();
    bool first ();
    bool isActive () const;
    bool isForwardOnly () const;
    bool isNull ( int field ) const;
    bool isSelect () const;
    bool isValid () const;
    bool last ();
    QSqlError lastError () const;
    QVariant lastInsertId () const;

    QString lastQuery () const;
    bool next ();
    bool nextResult ();
    int numRowsAffected () const;
    QSql::NumericalPrecisionPolicy numericalPrecisionPolicy () const;
    bool prepare ( const QString & query );
    bool previous ();
    QSqlRecord record () const;
    const QSqlResult *	result () const;
    bool seek ( int index, bool relative = false );
    void setForwardOnly ( bool forward );
    void setNumericalPrecisionPolicy ( QSql::NumericalPrecisionPolicy precisionPolicy );
    int size () const;
    QVariant value ( int index ) const;
    QSqlQuery & operator= ( const QSqlQuery & other );
    QSqlQuery *query();
private:
    QSqlQuery m_sqlQuery;
};

#endif // SQLQUERY_H
