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

#ifndef ORM_SQLEXCEPTION_H
#define ORM_SQLEXCEPTION_H

#include <exception>
#include <QString>
#include <QDebug>

class QSqlError;

/*!
 * \brief The SqlException class is a personalized version of std::exception using also QSqlError.
 */
class SqlException : public std::exception
{
public:
  SqlException();
  SqlException(QSqlError &);
  ~SqlException() throw() {}
  virtual QString what();
  virtual QString & errorText();
  virtual QString & databaseErrorText();
  virtual QString & driverErrorTex();
  virtual int databaseErrorNumber();
protected:
  QString m_errorText;
  QString m_databaseErrorText;
  QString m_driverErrorText;
  int m_databaseErrorNumber;
};

QDebug operator<<(QDebug debug, SqlException &sqlException);

#endif // ORM_SQLEXCEPTION_H
