#include <QJsonObject>
#include <QJsonArray>

#include "rfidmonitorsettings.h"

namespace json {

RFIDMonitorSettings::RFIDMonitorSettings()
{
}
int RFIDMonitorSettings::id() const
{
    return m_id;
}

void RFIDMonitorSettings::setId(int id)
{
    m_id = id;
}
QString RFIDMonitorSettings::name() const
{
    return m_name;
}

void RFIDMonitorSettings::setName(const QString &name)
{
    m_name = name;
}
QString RFIDMonitorSettings::macAddress() const
{
    return m_macAddress;
}

void RFIDMonitorSettings::setMacAddress(const QString &macAddress)
{
    m_macAddress = macAddress;
}

QList<Module> RFIDMonitorSettings::modules() const
{
    return m_modules;
}

void RFIDMonitorSettings::setModules(const QList<Module> &modules)
{
    m_modules = modules;
}
DefaultServices RFIDMonitorSettings::defaultServices() const
{
    return m_defaultServices;
}

void RFIDMonitorSettings::setDefaultServices(const DefaultServices &defaultServices)
{
    m_defaultServices = defaultServices;
}
Network RFIDMonitorSettings::networkConfiguration() const
{
    return m_networkConfiguration;
}

void RFIDMonitorSettings::setNetworkConfiguration(const Network &networkConfiguration)
{
    m_networkConfiguration = networkConfiguration;
}
QString RFIDMonitorSettings::serverAddress() const
{
    return m_serverAddress;
}

void RFIDMonitorSettings::setServerAddress(const QString &serverAddress)
{
    m_serverAddress = serverAddress;
}

QString RFIDMonitorSettings::device() const
{
    return m_device;
}

void RFIDMonitorSettings::setDevice(const QString &device)
{
    m_device = device;
}


void RFIDMonitorSettings::read(const QJsonObject &json)
{
    m_id = json["id"].toInt();
    m_name = json["name"].toString();
    m_macAddress = json["maccaddress"].toString();
    m_device = json["device"].toString();
    m_serverAddress = json["serveraddress"].toString();

    QJsonArray modules = json["modules"].toArray();
    for(int i=0; i < modules.size(); i++) {
        QJsonObject obj = modules[i].toObject();
        Module mod;
        mod.read(obj);
        m_modules.append(mod);
    }

    QJsonObject defaultServices = json["defaultservices"].toObject();
    m_defaultServices.read(defaultServices);

    QJsonObject network = json["network"].toObject();
    m_networkConfiguration.read(network);
}

void RFIDMonitorSettings::write(QJsonObject &json) const
{
    json["id"] = m_id;
    json["name"] = m_name;
    json["macaddress"] = m_macAddress;
    json["device"] = m_device;
    json["serveraddress"] = m_serverAddress;

    QJsonArray modules;
    foreach (Module mod, m_modules) {
        QJsonObject obj;
        mod.write(obj);
        modules.append(obj);
    }
    json["modules"] = modules;

    QJsonObject defaultServices;
    m_defaultServices.write(defaultServices);
    json["defaultservices"] = defaultServices;

    QJsonObject network;
    m_networkConfiguration.write(network);
    json["network"] = network;
}

int Service::serviceType() const
{
    return m_serviceType;
}

void Service::setServiceType(int serviceType)
{
    m_serviceType = serviceType;
}
QString Service::serviceName() const
{
    return m_serviceName;
}

void Service::setServiceName(const QString &serviceName)
{
    m_serviceName = serviceName;
}

void Service::read(const QJsonObject &json)
{
    m_serviceType = json["servicetype"].toInt();
    m_serviceName = json["servicename"].toString();
}

void Service::write(QJsonObject &json) const
{
    json["servicetype"] = m_serviceType;
    json["servicename"] = m_serviceName;
}


QList<Service> Module::services() const
{
    return m_services;
}

void Module::setServices(const QList<Service> &services)
{
    m_services = services;
}

bool Module::operator==(const Module &module)
{
    return m_moduleName == module.moduleName();
}

void Module::read(const QJsonObject &json)
{
    m_moduleName = json["modulename"].toString();
    m_version = json["version"].toInt();
    QJsonArray services = json["services"].toArray();
    for(int i=0; i < services.size(); i++) {
        QJsonObject obj = services[i].toObject();
        Service serv;
        serv.read(obj);
        m_services.append(serv);
    }
}

void Module::write(QJsonObject &json) const
{
    json["modulename"] = m_moduleName;
    json["version"] = m_version;
    QJsonArray services;
    foreach (Service serv, m_services) {
        QJsonObject obj;
        serv.write(obj);
        services.append(obj);
    }
    json["services"] = services;
}

Module::Module(const QString &name)
{
    m_moduleName = name;
    m_version = 0;
}

QString Module::moduleName() const
{
    return m_moduleName;
}

void Module::setModuleName(const QString &moduleName)
{
    m_moduleName = moduleName;
}
int Module::version() const
{
    return m_version;
}

void Module::setVersion(int version)
{
    m_version = version;
}




QString DefaultServices::persister() const
{
    return m_persister;
}

void DefaultServices::setPersister(const QString &persister)
{
    m_persister = persister;
}

QString DefaultServices::exporter() const
{
    return m_exporter;
}

void DefaultServices::setExporter(const QString &exporter)
{
    m_exporter = exporter;
}

QString DefaultServices::packager() const
{
    return m_packager;
}

void DefaultServices::setPackager(const QString &packager)
{
    m_packager = packager;
}

QString DefaultServices::synchronizer() const
{
    return m_synchronizer;
}

void DefaultServices::setSynchronizer(const QString &synchronizer)
{
    m_synchronizer = synchronizer;
}
QString DefaultServices::communicator() const
{
    return m_communicator;
}

void DefaultServices::setCommunicator(const QString &communicator)
{
    m_communicator = communicator;
}


void DefaultServices::read(const QJsonObject &json)
{
    m_reader = json["reader"].toString();
    m_persister = json["persister"].toString();
    m_exporter = json["exporter"].toString();
    m_packager = json["packager"].toString();
    m_synchronizer = json["synchronizer"].toString();
    m_communicator = json["communicator"].toString();
}

void DefaultServices::write(QJsonObject &json) const
{
    json["reader"] = m_reader;
    json["persister"] = m_persister;
    json["exporter"] = m_exporter;
    json["packager"] = m_packager;
    json["synchronizer"] = m_synchronizer;
    json["communicator"] = m_communicator;
}

QString DefaultServices::reader() const
{
    return m_reader;
}

void DefaultServices::setReader(const QString &reader)
{
    m_reader = reader;
}

QString Network::essid() const
{
    return m_essid;
}

void Network::setEssid(const QString &essid)
{
    m_essid = essid;
}
QString Network::password() const
{
    return m_password;
}

void Network::setPassword(const QString &password)
{
    m_password = password;
}

void Network::read(const QJsonObject &json)
{
    m_essid = json["essid"].toString();
    m_password = json["password"].toString();
}

void Network::write(QJsonObject &json) const
{
    json["essid"] = m_essid;
    json["password"] = m_password;
}



}

