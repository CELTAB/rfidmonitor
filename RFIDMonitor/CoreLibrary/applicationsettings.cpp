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

#include <QCoreApplication>

#include "coremodule.h"

#include "applicationsettings.h"

ApplicationSettings::ApplicationSettings(QObject *parent) :
    QObject(parent),
    m_servicesLoaded(false)
{
}

ApplicationSettings *ApplicationSettings::instance()
{
    static ApplicationSettings *singleton=0;
    if(!singleton){
        singleton = new ApplicationSettings(qApp);
    }
    return singleton;
}

void ApplicationSettings::installModule(CoreModule *module)
{
    // 1 - Check if module is not installed or is outdatedin JSON Configuration
    if(!(module->name().size()&&module->version())) {
        // if the condition above is true, the system updates the file
        foreach (Service *serv, module->services()) {
            // install all services
        }
        // update module version

    }
}

void ApplicationSettings::loadDefaultServices()
{
    // Load default Services on the system
    // Search for the default services in the JSON configuration file and try to load the instances into the map

}

bool ApplicationSettings::servicesLoaded()
{
    return m_servicesLoaded;
}

const QMap<ServiceType, QString> &ApplicationSettings::defaultServices() const
{
    return m_defaultServices;
}
