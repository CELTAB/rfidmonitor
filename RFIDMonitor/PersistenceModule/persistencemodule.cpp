/****************************************************************************
**
** WWW.FISHMONITORING.COM.BR
**
** Copyright (C) 2013
**                     Gustavo Valiati <gustavovaliati@gmail.com>
**                     Luis Valdes <luisvaldes88@gmail.com>
**                     Thiago R. M. Bitencourt <thiago.mbitencourt@gmail.com>
**
** This file is part of the FishMonitoring project
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; version 2
** of the License.
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

#include <QVariant>

#include "persistencemodule.h"
#include "data/dao/rfiddatadao.h"
#include "servicemanager.h"
#include "object/rfiddata.h"

#include "core/connectionpool.h"

/*!
 * \brief insert_object Offers the way to persist one Rfiddata object.
 *
 * This Function must be registered on the CoreLibrary module, to be accessible by another modules.
 *
 * \param data is the object to be inserted.
 * \return true if successfully inserted, false otherwise.
 */
bool insert_object(Rfiddata *data)
{
    return RfiddataDAO::instance()->insertObject(data);
}

/*!
 * \brief update_object_list Offers the way to update the whole list of Rfiddata objects.
 *
 *This Function must be registered on the CoreLibrary module, to be accessible by another modules.
 *
 * \param list receives a list of Rfiddata
 * \return true if successfully updated, false otherwise.
 */
bool update_object_list(const QList<Rfiddata *> &list)
{
    return RfiddataDAO::instance()->updateObjectList(list);
}

/*!
 * \brief select_data Offers the way to select a list of Rfiddata objects that matches the parameter.
 *
 * This Function must be registered on the CoreLibrary module, to be accessible by another modules.
 *
 * \param ColumnObject Representes the attributes of Rfiddata object at database, that will be restricted.
 * \param value The restrictive value of the \a ColumnObject.
 * \return QList<Rfiddata *> as the list of objects found in database.
 */
QList<Rfiddata *> select_data(const QString &ColumnObject, QVariant value)
{
    return RfiddataDAO::instance()->getByMatch(ColumnObject, value, 0);
}

PersistenceModule::PersistenceModule(QObject *parent) :
    CoreModule(parent)
{
    /*
     * The functions RfiddataDAO::instance() and ConnectionPool::instance() create
     * the unique instance these classes here, to preserve the life cycle of the object together
     * with the PersistenceModule.
     */
    RfiddataDAO::instance();
    ConnectionPool::instance();
}

void PersistenceModule::init()
{
    setObjectName("PersistenceModule");
    // Register these functions in CoreLibrary.
    ServiceManager::instance()->register_function("persistence.insert_object", std::function< bool(Rfiddata *data) >(insert_object));
    ServiceManager::instance()->register_function("persistence.update_object_list", std::function< bool(const QList<Rfiddata * > &) >(update_object_list));
    ServiceManager::instance()->register_function("persistence.select_data", std::function< QList<Rfiddata *> (const QString &, QVariant ) >(select_data));
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PersistenceModule, CoreModule)
#endif // QT_VERSION < 0x050000
