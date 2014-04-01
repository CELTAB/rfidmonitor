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

#include <QSqlError>
#include "sqlexception.h"

SqlException::SqlException()  :
    std::exception(),
    m_errorText("Error No Especificado"),
    m_databaseErrorText(),
    m_driverErrorText(),
    m_databaseErrorNumber(-1)
{
}

SqlException::SqlException(QSqlError &sqlError) :
       std::exception()
{
    m_errorText = sqlError.text();
    m_databaseErrorText = sqlError.databaseText();
    m_driverErrorText = sqlError.driverText();
    m_databaseErrorNumber = sqlError.number();
}

QString SqlException::what()
{
    return "SQLException -> Mensaje de Error Predeterminado";
}

QString &SqlException::errorText()
{
     return m_errorText;
}

QString &SqlException::databaseErrorText()
{
    return m_databaseErrorText;
}

QString &SqlException::driverErrorTex()
{
    return m_driverErrorText;
}

int SqlException::databaseErrorNumber()
{
    return m_databaseErrorNumber;
}


QDebug operator<<(QDebug debug, SqlException &sqlException){
    debug << sqlException.errorText();
    return debug;
}
