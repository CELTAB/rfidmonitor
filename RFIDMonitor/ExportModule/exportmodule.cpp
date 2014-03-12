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

#include <rfidmonitor.h>
#include "exportservice.h"
#include "exportmodule.h"
#include "logger.h"

ExportModule::ExportModule(QObject *parent) :
    CoreModule(parent)
{

}

void ExportModule::init()
{
    // register the service to exportation
    ExportService *exportService = new ExportService(this);
    addService(exportService->serviceName(), exportService);
    RFIDMonitor::instance()->setDefaultService(ServiceType::KExportService, exportService->serviceName());
}

QString ExportModule::name()
{
    return "exportUSB.thiago";
}

quint32 ExportModule::version()
{
    return 1;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(ExportModule, ExportModule)
#endif // QT_VERSION < 0x050000
