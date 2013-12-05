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

#ifndef PERSISTENCEMODULE_H
#define PERSISTENCEMODULE_H

/*!
 * \class PersistenceModule
 * \brief The PersistenceModule class is responsible to insert, update, delete and get RFIDData objects in database.
 *
 * The PersistenceModule class register some internal functions as public functions
 * to the CoreLibrary, to provide these functionities to another modules.
 */

#include <coremodule.h>

class PersistenceModule : public CoreModule
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.celtab.CoreModule" FILE "PersistenceModule.json")
#endif // QT_VERSION >= 0x050000
    
public:
    explicit PersistenceModule(QObject *parent=0);

    void init();
};

#endif // PERSISTENCEMODULE_H
