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

#include "communicationmodule.h"
#include "communicationservice.h"

CommunicationModule::CommunicationModule(QObject *parent) :
    CoreModule(parent)
{
}

CommunicationModule::~CommunicationModule()
{

}

void CommunicationModule::init()
{
    CommunicationService *commService = new CommunicationService(this);
    addService(commService->serviceName(), commService);
    RFIDMonitor::instance()->setDefaultService(ServiceType::KCommunicationService, commService->serviceName());
}

QString CommunicationModule::name()
{
    return "communication.module";
}

quint32 CommunicationModule::version()
{
    return 1;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(CommunicationModule, CoreModule)
#endif // QT_VERSION < 0x050000
