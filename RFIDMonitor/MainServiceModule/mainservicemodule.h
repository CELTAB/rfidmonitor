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

#ifndef MAINSERVICEMODULE_H
#define MAINSERVICEMODULE_H

#include <coremodule.h>

/*!
 * \brief The MainServiceModule class is the interface of the module that offers a service where the application will call other services.
 *
 * The service provided by this module receives the name of the device as a parameter.
 */
class MainServiceModule : public CoreModule
{

    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.celtab.CoreModule" FILE "MainServiceModule.json")
#endif // QT_VERSION >= 0x050000
    
public:
    explicit MainServiceModule(QObject *parent = 0);
    ~MainServiceModule();

    void init();
};

#endif // MAINSERVICEMODULE_H
