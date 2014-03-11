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

#include <QSharedPointer>

#include "servicemanager.h"
#include "core/service.h"

struct ServiceManagerPrivate
{
    QMap<QString, Service *> readingServices;
    QMap<QString, Service *> persistenceServices;
    QMap<QString, Service *> exportServices;
    QMap<QString, Service *> synchronizeServices;

    QMap<QString, Service *> services(ServiceType type)
    {
        switch (type) {
        case ServiceType::KReadingService:
            return readingServices;
            break;
        case ServiceType::KPersistenceService:
            return persistenceServices;
            break;
        case ServiceType::KExportService:
            return exportServices;
            break;
        case ServiceType::KSynchronizeService:
            return synchronizeServices;
            break;
        default:
            return QMap<QString, Service *>();
            break;
        }
    }
};

ServiceManager::ServiceManager(QObject *parent) :
    QObject(parent),
    d_ptr(new ServiceManagerPrivate)
{

}


ServiceManager *ServiceManager::instance()
{
    static QSharedPointer<ServiceManager> singleton(NULL);
    if(singleton.isNull()){
        singleton = QSharedPointer<ServiceManager>(new ServiceManager);
    }
    return singleton.data();
}

ServiceManager::~ServiceManager()
{
    delete d_ptr;
}

void ServiceManager::registerService(QString serviceName, Service *serviceInstance)
{
    switch (serviceInstance->type()) {
    case ServiceType::KReadingService:
        d_ptr->readingServices.insert(serviceName, serviceInstance);
        break;
    case ServiceType::KPersistenceService:
        d_ptr->persistenceServices.insert(serviceName, serviceInstance);
        break;
    case ServiceType::KExportService:
        d_ptr->exportServices.insert(serviceName, serviceInstance);
        break;
    case ServiceType::KSynchronizeService:
        d_ptr->synchronizeServices.insert(serviceName, serviceInstance);
        break;
    default:
        break;
    }
}

QMap<QString, Service *> ServiceManager::services(ServiceType type)
{
    return d_ptr->services(type);
}
