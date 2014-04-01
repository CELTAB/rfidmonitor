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

#ifndef CRUD_DAO_GENERICDAO_H
#define CRUD_DAO_GENERICDAO_H

#include <QObject>
#include <QSqlError>
#include <QDebug>

#define PARENT(parent) (parent != 0 ? parent : this)

class QSqlError;
class QSqlDatabase;

class ObjectBase;

template <class ClassName>
class GenericDAO : public QObject
{
public:

    virtual bool insertObject(ClassName *obj) = 0;
    virtual bool updateObject(ClassName *obj) = 0;
    virtual bool deleteObject(ClassName *obj) = 0;

    virtual ClassName * getById(qlonglong id, QObject *parent=0) = 0;
    virtual QList<ClassName *> getAll(QObject *parent=0) = 0;
    virtual QList<ClassName *> getByMatch(const QString &, QVariant , QObject *parent=0) = 0;

protected:
    explicit GenericDAO(QObject *parent) :
        QObject(parent)
    {
    }

};

#endif // CRUD_DAO_GENERICDAO_H
