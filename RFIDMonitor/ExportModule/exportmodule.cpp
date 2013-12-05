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

#include <servicemanager.h>

#include "exportmodule.h"
#include "export/exportlocaldata.h"

static DeviceThread *g_thread = 0;

/*!
 * \brief start_daemon start Thread to detect new devices to export data
 */
void start_daemon(){
    // only start a new thread if this is not already started
    if(!g_thread){
        g_thread = new DeviceThread;
        g_thread->start();
    }
}

ExportModule::ExportModule(QObject *parent) :
    CoreModule(parent)
{
    /* create a unique instance for the class ExportLocalData, that will be called from DeviceThread class.
    * If not do this, will get an error when the DeviceThread class try use an instance of ExportLocalData, because
    * is trying to create an object from ExportLocalData class into a thread that is child of the thread that is owner of the ExportLocalData class, so this is not possible
    */
    ExportLocalData::instance();
}

ExportModule::~ExportModule()
{
    g_thread->exit(0); // stop and exit the thread
    g_thread->deleteLater(); // delete the thread from memory
}

void ExportModule::init()
{
    // register the service to start a thread to detect devices
    ServiceManager::instance()->register_function("export.start_daemon", std::function< void()>(start_daemon));
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(ExportModule, ExportModule)
#endif // QT_VERSION < 0x050000
