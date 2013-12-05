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

#include <QMap>

#include <servicemanager.h>

#include "mainservicemodule.h"
#include "mainservice.h"

static MainService *mainService = 0;

/*!
 * \brief call_main_service receives te name of the device that the system is going to use as an input stream.
 * \param device
 */
void call_main_service(const QString &device)
{
    if(!mainService)
        mainService = new MainService;

    QMap<QString, QString> services;
    services.insert(MainService::KDeviceParam, device);
    mainService->callMainServices(services);
}

MainServiceModule::MainServiceModule(QObject *parent) :
    CoreModule(parent)
{
}

MainServiceModule::~MainServiceModule()
{
    mainService->deleteLater();
}

void MainServiceModule::init()
{
    // Register the service in the ServiceManager
    ServiceManager::instance()->register_function("main.call_services", std::function<void (const QString &)>(call_main_service));
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(MainServiceModule, MainService)
#endif // QT_VERSION < 0x050000
