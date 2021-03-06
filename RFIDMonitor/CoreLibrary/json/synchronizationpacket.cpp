#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

#include "synchronizationpacket.h"

namespace json {

DataSummary SynchronizationPacket::dataContent() const
{
    return m_dataSummary;
}

void SynchronizationPacket::setDataContent(const DataSummary &dataContent)
{
    m_dataSummary = dataContent;
}

void SynchronizationPacket::read(const QJsonObject &json)
{
#if QT_VERSION < 0x050200
    m_id = json["id"].toVariant().toInt();
#else
    m_id = json["id"].toInt();
#endif // QT_VERSION < 0x050200
    m_name = json["name"].toString();
    m_macAddress = json["macaddress"].toString();
    QJsonObject dataSummaryObj = json["datasummary"].toObject();
    m_dataSummary.read(dataSummaryObj);
}

void SynchronizationPacket::write(QJsonObject &json) const
{
    json["id"] = m_id;
    json["name"] = m_name;
    json["macaddress"] = m_macAddress;
    QJsonObject dataSummary;
    m_dataSummary.write(dataSummary);
    json["datasummary"] = dataSummary;
}

QString SynchronizationPacket::macAddress() const
{
    return m_macAddress;
}

void SynchronizationPacket::setMacAddress(const QString &macAddress)
{
    m_macAddress = macAddress;
}

QString SynchronizationPacket::name() const
{
    return m_name;
}

void SynchronizationPacket::setName(const QString &name)
{
    m_name = name;
}

int SynchronizationPacket::id() const
{
    return m_id;
}

void SynchronizationPacket::setId(int id)
{
    m_id = id;
}


int Data::id() const
{
    return m_id;
}

void Data::setId(int id)
{
    m_id = id;
}
int Data::idcollectorPoint() const
{
    return m_idcollectorPoint;
}

void Data::setIdcollectorPoint(int idcollectorPoint)
{
    m_idcollectorPoint = idcollectorPoint;
}
int Data::idantena() const
{
    return m_idantena;
}

void Data::setIdantena(int idantena)
{
    m_idantena = idantena;
}
QDateTime Data::dateTime() const
{
    return m_dateTime;
}

void Data::setDateTime(const QDateTime &dateTime)
{
    m_dateTime = dateTime;
}

void Data::read(const QJsonObject &json)
{
#if QT_VERSION < 0x050200
    m_id = json["id"].toVariant().toInt();
    m_idcollectorPoint = json["idcollectorpoint"].toVariant().toInt();
    m_idantena = json["idantena"].toVariant().toInt();
//    m_applicationCode = json["applicationcode"].toVariant().toInt();
//    m_identificationCode = json["identificationcode"].toVariant().toInt();
    m_applicationCode = json["applicationcode"].toVariant().toLongLong();
    m_identificationCode = json["identificationcode"].toVariant().toLongLong();
#else
    m_id = json["id"].toInt();
    m_idcollectorPoint = json["idcollectorpoint"].toInt();
    m_idantena = json["idantena"].toInt();
    m_applicationCode = json["applicationcode"].toString().toLongLong();
    m_identificationCode = json["identificationcode"].toString().toLongLong();
#endif // QT_VERSION < 0x050200
    QString dateTime = json["datetime"].toString();
    m_dateTime = QDateTime::fromString(json["datetime"].toString(), Qt::ISODate);
}

void Data::write(QJsonObject &json) const
{
    json["id"] = m_id;
    json["idcollectorpoint"] = m_idcollectorPoint;
    json["idantena"] = m_idantena;
//#if QT_VERSION < 0x050200
//    json["applicationcode"] = (int)m_applicationCode;
//    json["identificationcode"] = (int)m_identificationCode;
//#else
    json["applicationcode"] = QString::number(m_applicationCode);
    json["identificationcode"] = QString::number(m_identificationCode);
//#endif // QT_VERSION < 0x050200
    QString dateTime = m_dateTime.toString(Qt::ISODate);
    json["datetime"] = dateTime;
}

qlonglong Data::applicationCode() const
{
    return m_applicationCode;
}

void Data::setApplicationCode(qlonglong applicationCode)
{
    m_applicationCode = applicationCode;
}

qlonglong Data::identificationCode() const
{
    return m_identificationCode;
}

void Data::setIdentificationCode(qlonglong identificationCode)
{
    m_identificationCode = identificationCode;
}





int DataSummary::idEnd() const
{
    return m_idEnd;
}

void DataSummary::setIdEnd(int idEnd)
{
    m_idEnd = idEnd;
}

QString DataSummary::md5diggest() const
{
    return m_md5diggest;
}

void DataSummary::setMd5diggest(const QString &md5diggest)
{
    m_md5diggest = md5diggest;
}

QList<Data> DataSummary::data() const
{
    return m_data;
}

void DataSummary::setData(const QList<Data> &data)
{
    m_data = data;
}

void DataSummary::read(const QJsonObject &json)
{
#if QT_VERSION < 0x050200
    m_idBegin = json["idbegin"].toVariant().toInt();
    m_idEnd = json["idend"].toVariant().toInt();
#else
    m_idBegin = json["idbegin"].toInt();
    m_idEnd = json["idend"].toInt();
#endif // QT_VERSION < 0x050200
    m_md5diggest = json["md5diggest"].toString();
    QJsonArray dataArray = json["data"].toArray();
    for(int i = 0; i < dataArray.size(); i++){
        QJsonObject obj = dataArray[i].toObject();
        Data data;
        data.read(obj);
        m_data.append(data);
    }
}

void DataSummary::write(QJsonObject &json) const
{
    json["idbegin"] = m_idBegin;
    json["idend"] = m_idEnd;
    json["md5diggest"] = m_md5diggest;
    QJsonArray dataArray;
    foreach (const Data data, m_data) {
        QJsonObject obj;
        data.write(obj);
        dataArray.append(obj);
    }
    json["data"] = dataArray;
}
int DataSummary::idBegin() const
{
    return m_idBegin;
}

void DataSummary::setIdBegin(int idBegin)
{
    m_idBegin = idBegin;
}

QString Packet::md5diggest() const
{
    return m_md5diggest;
}

void Packet::setMd5diggest(const QString &md5diggest)
{
    m_md5diggest = md5diggest;
}
QString Packet::status() const
{
    return m_status;
}

void Packet::setStatus(const QString &status)
{
    m_status = status;
}

void Packet::read(const QJsonObject &json)
{
    m_md5diggest = json["md5diggest"].toString();
    m_status = json["status"].toString();
}

void Packet::write(QJsonObject &json) const
{
    json["md5diggest"] = m_md5diggest;
    json["status"] = m_status;
}


int SynchronizationCheck::id() const
{
    return m_id;
}

void SynchronizationCheck::setId(int id)
{
    m_id = id;
}
QString SynchronizationCheck::name() const
{
    return m_name;
}

void SynchronizationCheck::setName(const QString &name)
{
    m_name = name;
}
QString SynchronizationCheck::macAddress() const
{
    return m_macAddress;
}

void SynchronizationCheck::setMacAddress(const QString &macAddress)
{
    m_macAddress = macAddress;
}
QList<Packet> SynchronizationCheck::packets() const
{
    return m_packets;
}

void SynchronizationCheck::setPackets(const QList<Packet> &packets)
{
    m_packets = packets;
}


void SynchronizationCheck::read(const QJsonObject &json)
{
//#if QT_VERSION < 0x050200
//    m_id = json["id"].toVariant().toInt();
//#else
//    m_id = json["id"].toInt();
//#endif // QT_VERSION < 0x050200
//    m_name = json["name"].toString();
//    m_macAddress = json["macaddress"].toString();
    QJsonArray packets = json["packets"].toArray();
    for(int i=0; i < packets.size(); i++){
        QJsonObject obj = packets[i].toObject();
        Packet pkt;
        pkt.read(obj);
        m_packets.append(pkt);
    }
}

void SynchronizationCheck::write(QJsonObject &json) const
{
    json["id"] = m_id;
    json["name"] = m_name;
    json["macaddress"] = m_macAddress;
    QJsonArray packets;
    foreach (Packet pkt, m_packets) {
        QJsonObject obj;
        pkt.write(obj);
        packets.append(obj);
    }
    json["packets"] = packets;
}



}


