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
    return ServiceType::KPackager;
}

/*

{"id": 1,
"name": "celtab",
"mac": "78:2b:cb:c0:76:5e",
"ip": "179.106.217.28",
"packets": [ {
"idinicial": 1,
"idfinal": 100,
"count": 100,
"randomindex": 1408,
"md5diggest": "78:2b:cb:c0:76:5e",

"data": [{

"id": 100,
"idpontocoleta": 122,
"idantena": 3,
"applicationcode": 11099182342,
"identificationcode": 0,
"datetime": "2014-03-14T11:32:12"

}]

}]}

*/
static QString jsonAtribute(const QString &attr, const QString &value = "")
{
    return QString("\"%1\": %2").arg(attr).arg(value);
}

QMap<QString, QByteArray> PackagerService::getAll()
{
    static PersistenceInterface *persitence = 0;
    if(!persitence){
        persitence = qobject_cast<PersistenceInterface *>(RFIDMonitor::instance()->defaultService(ServiceType::KPersister));
    }
    QList<Rfiddata *> data = persitence->getObjectList("sync", QVariant("0"), 0);
    int stagesCount = data.size()%100;

    int currentIndex = 0;
    QMap<QString, QByteArray> package;
    for(int stage = 0; stage < stagesCount; stage++){
        QByteArray message;
        // Begin Object
        QString jsonData = "{";
        // General Information
        jsonData.append(jsonAtribute("id", "1").append(","));
        jsonData.append(jsonAtribute("name", "\"celtab\"").append(","));
        jsonData.append(jsonAtribute("mac", "\"78:2b:cb:c0:76:5e\"").append(","));
        jsonData.append(jsonAtribute("ip", "\"179.106.217.28\"").append(","));


        QString jsonPackets= "";
        jsonPackets.append(jsonAtribute("data"));
        // Begin Data Array
        jsonPackets.append(" [ ");
        for(; currentIndex < data.size()/(stagesCount/(stage+1)); currentIndex++){
            jsonPackets.append("{");
            jsonPackets.append(jsonAtribute("pontocoleta", data.at(currentIndex)->idpontocoleta().toString()).append(","));
            jsonPackets.append(jsonAtribute("antena", data.at(currentIndex)->idantena().toString()).append(","));
            jsonPackets.append(jsonAtribute("identificationcode", data.at(currentIndex)->identificationcode().toString()).append(","));
            jsonPackets.append(jsonAtribute("applicationcode", data.at(currentIndex)->applicationcode().toString()).append(","));
            jsonPackets.append(jsonAtribute("datetime", QString("\"%1\"").arg(data.at(currentIndex)->datetime().toDateTime().toString(Qt::ISODate))));
            jsonPackets.append("}").append(currentIndex != (data.size()/(stagesCount/(stage+1))-1) ? "," : "");
        }
        // Close Data Array
        jsonPackets.append("]");


        QByteArray hash = QCryptographicHash::hash(jsonPackets.toLatin1(), QCryptographicHash::Md5).toHex();

        jsonData.append(jsonAtribute("packets"));

        // Begin Packets Array
        jsonData.append("[");
        // Begin Data Array Content
        jsonData.append("{");
        jsonData.append(jsonAtribute("md5diggest", QString("\"%1\"").arg(QString(hash)).append(",")));
        jsonData.append(jsonPackets);
        // End Data Array Content
        jsonData.append("}");

        // Close Packets Array
        jsonData.append("]");

        // End Object
        jsonData.append("}");

        message.append(jsonData);
        package.insert(QString(hash), message);
    }

    foreach (Rfiddata *rf, data) {
        rf->deleteLater();
    }

    return package;
}
