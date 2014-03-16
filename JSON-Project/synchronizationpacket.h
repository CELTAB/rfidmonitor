#ifndef SYNCHRONIZATIONPACKET_H
#define SYNCHRONIZATIONPACKET_H

#include <QObject>

class QJsonObject;

class JsonRWInterface
{
public:
    virtual void read(const QJsonObject &json) = 0;
    virtual void write(QJsonObject &json) const = 0;
};

class Data : public JsonRWInterface
{
public:
    int id() const;
    void setId(int id);

    int idcollectorPoint() const;
    void setIdcollectorPoint(int idcollectorPoint);

    int idantena() const;
    void setIdantena(int idantena);

    int identificationCode() const;
    void setIdentificationCode(int identificationCode);

    int applicationCode() const;
    void setApplicationCode(int applicationCode);

    QDateTime dateTime() const;
    void setDateTime(const QDateTime &dateTime);

private:
    int m_id;
    int m_idcollectorPoint;
    int m_idantena;
    int m_identificationCode;
    int m_applicationCode;
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

    QString ipAddress() const;
    void setIpAddress(const QString &ipAddress);

    DataSummary dataContent() const;
    void setDataContent(const DataSummary &dataContent);
private:
    int m_id;
    QString m_name;
    QString m_macAddress;
    QString m_ipAddress;
    DataSummary m_dataSummary;

    // JsonRWInterface interface
public:
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
};

#endif // SYNCHRONIZATIONPACKET_H
