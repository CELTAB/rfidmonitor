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

#include <QCryptographicHash>

#include <rfidmonitor.h>
#include <object/rfiddata.h>

#include "packagerservice.h"

PackagerService::PackagerService(QObject *parent) :
    PackagerInterface(parent)
{
}

QString PackagerService::serviceName() const
{
    return "packager.service";
}

void PackagerService::init()
{

}

ServiceType PackagerService::type()
{
    return ServiceType::KPackagerService;
}

QMap<QString, QByteArray> PackagerService::getAll()
{
    static PersistenceInterface *persitence = 0;
    if(!persitence){
        persitence = qobject_cast<PersistenceInterface *>(RFIDMonitor::instance()->defaultService(ServiceType::KPersistenceService));
    }
    QList<Rfiddata *> data = persitence->getObjectList("sync", QVariant("0"), 0);
    int stagesCount = data.size()%100;

    int currentIndex = 0;
    QMap<QString, QByteArray> package;
    for(int stage = 0; stage < stagesCount; stage++){
        QByteArray message;
        QString jsonData = "";
        for(; currentIndex < data.size(); currentIndex++){
            jsonData.append("{");
            jsonData.append(QString("pontocoleta: %1,").arg(data.at(currentIndex)->idpontocoleta().toString()));
            jsonData.append(QString("antena: %1,").arg(data.at(currentIndex)->idantena().toString()));
            jsonData.append(QString("applicationcode: %1,").arg(data.at(currentIndex)->applicationcode().toString()));
            jsonData.append(QString("identificationcode: %1,").arg(data.at(currentIndex)->identificationcode().toString()));
            jsonData.append(QString("datetime: %1").arg(data.at(currentIndex)->datetime().toDateTime().toString(Qt::ISODate)));
            jsonData.append("}");
        }
        message.append(jsonData);
        QByteArray hash = QCryptographicHash::hash(message, QCryptographicHash::Md5);
//        message.append(hash);
        package.insert(QString(hash), message);
    }

    foreach (Rfiddata *rf, data) {
        rf->deleteLater();
    }

    return package;
}
