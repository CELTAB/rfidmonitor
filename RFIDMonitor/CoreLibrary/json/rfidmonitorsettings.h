#ifndef RFIDMONITORSETTINGS_H
#define RFIDMONITORSETTINGS_H

#include "jsonrwinterface.h"
#include <QDebug>

namespace json {

class Service : public JsonRWInterface
{
public:

    int serviceType() const;
    void setServiceType(int serviceType);

    QString serviceName() const;
    void setServiceName(const QString &serviceName);

private:
    int m_serviceType;
    QString m_serviceName;

    // JsonRWInterface interface
public:
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
};

class Module : public JsonRWInterface
{
public:
    explicit Module(const QString &name = "");
    QString moduleName() const;
    void setModuleName(const QString &moduleName);

    int version() const;
    void setVersion(int version);

    QList<Service> services() const;
    void setServices(const QList<Service> &services);

    bool operator==(const Module &module);

private:
    QString m_moduleName;
    int m_version;
    QList<Service> m_services;

    // JsonRWInterface interface
public:
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
};

class DefaultServices : public JsonRWInterface
{

public:
    QString reader() const;
    void setReader(const QString &reader);
    QString persister() const;
    void setPersister(const QString &persister);
    QString exporter() const;
    void setExporter(const QString &exporter);
    QString packager() const;
    void setPackager(const QString &packager);
    QString synchronizer() const;
    void setSynchronizer(const QString &synchronizer);

    QString communicator() const;
    void setCommunicator(const QString &communicator);

private:
    QString m_reader;
    QString m_persister;
    QString m_exporter;
    QString m_packager;
    QString m_synchronizer;
    QString m_communicator;

    // JsonRWInterface interface
public:
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
};

class Network : public JsonRWInterface
{
public:

    QString essid() const;
    void setEssid(const QString &essid);

    QString password() const;
    void setPassword(const QString &password);

private:
    QString m_essid;
    QString m_password;

    // JsonRWInterface interface
public:
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
};

class RFIDMonitorSettings : public JsonRWInterface
{
public:
    RFIDMonitorSettings();

    int id() const;
    void setId(int id);

    QString name() const;
    void setName(const QString &name);

    QString macAddress() const;
    void setMacAddress(const QString &macAddress);

    QList<Module> modules() const;
    void setModules(const QList<Module> &modules);

    DefaultServices defaultServices() const;
    void setDefaultServices(const DefaultServices &defaultServices);

    Network networkConfiguration() const;
    void setNetworkConfiguration(const Network &networkConfiguration);

    QString device() const;
    void setDevice(const QString &device);

    QString serverAddress() const;
    void setServerAddress(const QString &serverAddress);

    int serverPort() const;
    void setServerPort(const int &serverPort);

private:
    int m_id;
    int m_serverPort;
    QString m_name;
    QString m_macAddress;
    QString m_device;
    QString m_serverAddress;

    QList<Module> m_modules;
    DefaultServices m_defaultServices;
    Network m_networkConfiguration;

    // JsonRWInterface interface
public:
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
};

}


#endif // RFIDMONITORSETTINGS_H
