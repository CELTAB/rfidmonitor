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
#include <logger.h>

#include "reader_rfm008bmodule.h"
#include "reader_rfm008b.h"

Reader_RFM008BModule::Reader_RFM008BModule(QObject *parent) :
    CoreModule(parent)
{
}

Reader_RFM008BModule::~Reader_RFM008BModule()
{

}

void Reader_RFM008BModule::init()
{
    Reader_RFM008B *reader = new Reader_RFM008B(this);
    addService(reader->serviceName(), reader);
    RFIDMonitor::instance()->setDefaultService(ServiceType::KReader, reader->serviceName());
}

QString Reader_RFM008BModule::name()
{
    return "reader_rfm008b.module";
}

quint32 Reader_RFM008BModule::version()
{
    return 1;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Reader_RFM008BModule, CoreModule)
#endif // QT_VERSION < 0x050000
