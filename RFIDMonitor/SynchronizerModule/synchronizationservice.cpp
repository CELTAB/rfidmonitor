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

#include "synchronizationservice.h"

SynchronizationService::SynchronizationService(QObject *parent) :
    SynchronizationInterface(parent)
{
}

void SynchronizationService::readyRead()
{
    Logger::instance()->writeRecord(Logger::fatal, "synchronizer", Q_FUNC_INFO, "Entering");
    static PackagerInterface *packager = 0;
    static CommunicationInterface *communitacion = 0;
    if(!packager || !communitacion) {
        packager = qobject_cast<PackagerInterface *>(RFIDMonitor::instance()->defaultService(ServiceType::KPackager));
        communitacion = qobject_cast<CommunicationInterface *>(RFIDMonitor::instance()->defaultService(ServiceType::KCommunicator));
    }
    if(packager) {
        QMap<QString, QByteArray> allData = packager->getAll();

        if(communitacion) {
            QMap<QString, QByteArray>::iterator i;
            for(i = allData.begin(); i != allData.end(); ++i){
                qDebug() << QString("Sending packet: %1 - size: %2").arg(i.key()).arg(i.value().size());
                communitacion->sendMessage(i.value());
            }
        }
    }
}

QString SynchronizationService::serviceName() const
{
    return "synchronization.service";
}

void SynchronizationService::init()
{

}

ServiceType SynchronizationService::type()
{
    return ServiceType::KSynchronizer;
}

void SynchronizationService::onDataReceived(QString data)
{
    (void)data;
}
