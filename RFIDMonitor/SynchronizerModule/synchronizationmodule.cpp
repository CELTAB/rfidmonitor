/****************************************************************************
**
** WWW.FISHMONITORING.COM.BR
**
** Copyright (C) 2013
**                     Luis Valdes <luisvaldes88@gmail.com>
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

#include <rfidmonitor.h>
#include <logger.h>

#include "synchronizationmodule.h"
#include "synchronizationservice.h"
#include "packagerservice.h"

SynchronizationModule::SynchronizationModule(QObject *parent) :
    CoreModule(parent)
{
}

SynchronizationModule::~SynchronizationModule()
{

}

void SynchronizationModule::init()
{
    PackagerService *packager = new PackagerService(this);
    SynchronizationService *syncService = new SynchronizationService(this);
    addService(packager->serviceName(), packager);
    addService(syncService->serviceName(), syncService);
    RFIDMonitor::instance()->setDefaultService(ServiceType::KPackager, packager->serviceName());
    RFIDMonitor::instance()->setDefaultService(ServiceType::KSynchronizer, syncService->serviceName());
}

QString SynchronizationModule::name()
{
    return "synchronization.module";
}

quint32 SynchronizationModule::version()
{
    return 1;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(SynchronizationModule, CoreModule)
#endif // QT_VERSION < 0x050000
