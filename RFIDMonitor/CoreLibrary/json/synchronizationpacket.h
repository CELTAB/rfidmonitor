#ifndef SYNCHRONIZATIONPACKET_H
#define SYNCHRONIZATIONPACKET_H

#include "jsonrwinterface.h"

namespace json {

class Data : public JsonRWInterface
{
public:
    int id() const;
    void setId(int id);

    qlonglong rfidCode() const;
    void setRfidCode(qlonglong rfidCode);

    QDateTime dateTime() const;
    void setDateTime(const QDateTime &dateTime);

    QJsonObject extraData() const;
    void setExtraData(const QJsonObject extraData);

private:
    int m_id;
    qlonglong m_rfidcode;
    QJsonObject m_extraData;
    QDateTime m_dateTime;

    // JsonRWInterface interface
public:
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
};

class DataSummary : public JsonRWInterface
{
public:
    QString md5diggest() const;
    void setMd5diggest(const QString &md5diggest);

    QList<Data> data() const;
    void setData(const QList<Data> &data);

private:
    QString m_md5diggest;
    QList<Data> m_data;

    // JsonRWInterface interface
public:
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
};

class SynchronizationPacket : public JsonRWInterface
{
public:
    int id() const;
    void setId(int id);

    QString name() const;
    void setName(const QString &name);

    QString macAddress() const;
    void setMacAddress(const QString &macAddress);

    DataSummary dataContent() const;
    void setDataContent(const DataSummary &dataContent);
private:
    int m_id;
    QString m_name;
    QString m_macAddress;
    DataSummary m_dataSummary;

    // JsonRWInterface interface
public:
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
};

class Packet : public JsonRWInterface
{
public:

    QString md5diggest() const;
    void setMd5diggest(const QString &md5diggest);

    QString status() const;
    void setStatus(const QString &status);

private:
    QString m_md5diggest;
    QString m_status;

    // JsonRWInterface interface
public:
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
};

class SynchronizationCheck : public JsonRWInterface
{
public:

    int id() const;
    void setId(int id);

    QString name() const;
    void setName(const QString &name);

    QString macAddress() const;
    void setMacAddress(const QString &macAddress);

    QList<Packet> packets() const;
    void setPackets(const QList<Packet> &packets);

private:
    int m_id;
    QString m_name;
    QString m_macAddress;
    QList<Packet> m_packets;

    // JsonRWInterface interface
public:
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
};


}


#endif // SYNCHRONIZATIONPACKET_H
