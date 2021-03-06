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
#include <QNetworkInterface>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <functional>
#include <future>

#include <rfidmonitor.h>
#include <object/rfiddata.h>

#include <json/synchronizationpacket.h>
#include <logger.h>

#include "packagerservice.h"

#include "data/dao/packetdao.h"
#include "data/object/packet.h"

QString getMacAddress()
{
    foreach(QNetworkInterface netInterface, QNetworkInterface::allInterfaces())
    {
        // Return only the first non-loopback MAC Address
        if (!(netInterface.flags() & QNetworkInterface::IsLoopBack))
            return netInterface.hardwareAddress();
    }
    return QString();
}

PackagerService::PackagerService(QObject *parent) :
    PackagerInterface(parent)
{
    collectorId = 0;
    collectorName = "";
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

QMap<QString, QByteArray> PackagerService::getAll()
{
    //    generatePackets();

    collectorId = RFIDMonitor::instance()->idCollector();
    collectorName = RFIDMonitor::instance()->collectorName();

    QList<Packet *> packetListNew = PacketDAO::instance()->getByMatch("status", (int)Packet::Status::KNew);

    QMap<QString, QByteArray> packets;
    // insert in the packets all data with KNew status
    Logger::instance()->writeRecord(Logger::severity_level::debug, "PackagerService", Q_FUNC_INFO, "Inserting on packets.");
    foreach (Packet *p, packetListNew) {
        packets.insert(p->md5hash().toString(), p->jsonData().toByteArray());
        Logger::instance()->writeRecord(Logger::severity_level::debug, "PackagerService", Q_FUNC_INFO, p->md5hash().toString());
    }

    //----  TEMP - DON'T REMOVE
    if(RFIDMonitor::instance()->isconnected()){
        QList<Packet *> packetListPending = PacketDAO::instance()->getByMatch("status", (int)Packet::Status::KConfimationPending);
        // Alson insert the packets with KConfirmationPending status to retry send it to the server.
        foreach (Packet *pa, packetListPending) {
            packets.insert(pa->md5hash().toString(), pa->jsonData().toByteArray());
        }
    }

    if(!packetListNew.isEmpty()){
        /*
         * WTF warning. This "if(!packetListNew.isEmpty())" has been added because the foreach was executing deleteLater on a phanton object.
         * Only God knows why.
         * Dont believe? remove this 'if' check the size of the list and let 'pack->deleteLater()' run to see the magic.
        */
        foreach (Packet *pack, packetListNew) {
            pack->setStatus((int)Packet::Status::KConfimationPending);
            PacketDAO::instance()->updateObject(pack);
            pack->deleteLater();
        }
    }

    return packets;
}

void PackagerService::update(const QList<QString> &list)
{
    foreach (QString hash, list) {
        QList<Packet *> packetList = PacketDAO::instance()->getByMatch("md5hash", hash);
        QList<int> idList;
        foreach (Packet *packet, packetList) {
            QJsonObject obj = QJsonDocument::fromJson(packet->jsonData().toByteArray()).object();
            json::SynchronizationPacket syncPacket;
            syncPacket.read(obj);
            foreach (const json::Data &data, syncPacket.dataContent().data()) {
                idList.append(data.id());
            }
            PacketDAO::instance()->deleteRFIDDataList(packet->md5hash().toString(),idList);
        }
    }
}

void PackagerService::generatePackets()
{
    QMutexLocker locker(&m_mutex);

    //    Logger::instance()->writeRecord(Logger::severity_level::debug, "PackagerService", Q_FUNC_INFO, "Generating packets...");
    static PersistenceInterface *persistence = 0;
    if(!persistence){
        persistence = qobject_cast<PersistenceInterface *>(RFIDMonitor::instance()->defaultService(ServiceType::KPersister));
    }
    QList<Rfiddata *> data = persistence->getObjectList("sync", QVariant(Rfiddata::KNotSynced), 0);
    int stagesCount = data.size()%100;


    int currentIndex = 0;
    QMap<QString, QByteArray> package;
    for(int stage = 0; stage < stagesCount; stage++){

        json::SynchronizationPacket synPacket;

        synPacket.setMacAddress(getMacAddress());
        synPacket.setName(collectorName);
        synPacket.setId(collectorId);

        json::DataSummary summary;
        QList<json::Data> rfidList;
        for(; currentIndex < data.size()/(stagesCount/(stage+1)); currentIndex++){
            data.at(currentIndex)->setSync(Rfiddata::KSynced);
            json::Data d;
            d.setId(data[currentIndex]->id().toInt());
            d.setIdcollectorPoint(data[currentIndex]->idpontocoleta().toInt());
            d.setIdantena(data[currentIndex]->idantena().toInt());
            d.setIdentificationCode(data[currentIndex]->identificationcode().toLongLong());
            d.setApplicationCode(data[currentIndex]->applicationcode().toLongLong());
            d.setDateTime(data[currentIndex]->datetime().toDateTime());
            rfidList.append(d);
        }
        summary.setData(rfidList);
        // In this section we create the md5 hash of the data array
        QJsonObject jsonSummary;
        summary.write(jsonSummary);
        QByteArray byteData(QJsonDocument(jsonSummary["data"].toArray()).toJson());
        QByteArray hash = QCryptographicHash::hash(byteData, QCryptographicHash::Md5).toHex();
        summary.setMd5diggest(hash);
        // Set te summary for the synchronizationPacket
        synPacket.setDataContent(summary);

        QJsonObject packet;
        synPacket.write(packet);
        // The packet must be sent as a bytearray and the packet itself is identified by an md5 hash
        QByteArray document = QJsonDocument(packet).toJson();

        package.insert(QString(hash), document);
    }

    persistence->updateObjectList(data);
    QMap<QString, QByteArray>::iterator it;
    for(it = package.begin(); it != package.end(); it++){
        Packet pack;
        pack.setMd5hash(it.key());
        pack.setDateTime(QDateTime::currentDateTime());
        pack.setIdBegin(0);
        pack.setItemCount(10);
        pack.setJsonData(it.value());
        pack.setStatus((int)Packet::Status::KNew);
        PacketDAO::instance()->insertObject(&pack);
    }

    foreach (Rfiddata *rf, data) {
        rf->deleteLater();
    }
}
