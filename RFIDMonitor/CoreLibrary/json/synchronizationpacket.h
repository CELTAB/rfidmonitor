#ifndef SYNCHRONIZATIONPACKET_H
#define SYNCHRONIZATIONPACKET_H

#include "jsonrwinterface.h"

namespace json {

class Data : public JsonRWInterface
{
public:
    int id() const;
    void setId(int id);

    int idcollectorPoint() const;
    void setIdcollectorPoint(int idcollectorPoint);

    int idantena() const;
    void setIdantena(int idantena);

    qlonglong identificationCode() const;
    void setIdentificationCode(qlonglong identificationCode);

    qlonglong applicationCode() const;
    void setApplicationCode(qlonglong applicationCode);

    QDateTime dateTime() const;
    void setDateTime(const QDateTime &dateTime);

private:
    int m_id;
    int m_idcollectorPoint;
    int m_idantena;
    qlonglong m_identificationCode;
    qlonglong m_applicationCode;
    QDateTime m_dateTime;

    // JsonRWInterface interface
public:
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
};

class DataSummary : public JsonRWInterface
{
public:
    int idBegin() const;
    void setIdBegin(int idBegin);

    int idEnd() const;
    void setIdEnd(int idEnd);

    QString md5diggest() const;
    void setMd5diggest(const QString &md5diggest);

    QList<Data> data() const;
    void setData(const QList<Data> &data);

private:
    int m_idBegin;
    int m_idEnd;
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
