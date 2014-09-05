#include "settings.h"

Settings::Settings() :
m_settings("settings.ini", QSettings::NativeFormat)
{

}

Settings *Settings::instance()
{
    static Settings *instance = 0;
    if(!instance)
        instance = new Settings;
    return instance;
}

QString Settings::stringifyServiceType(Settings::ServiceType type)
{
    switch (type) {
    case ServiceType::KReaderService:
        return QString("Reader");
    case ServiceType::KPersister:
        return QString("Persister");
    case ServiceType::KExporter:
        return QString("Exporter");
    case ServiceType::KSynchronizer:
        return QString("Synchronizer");
    case ServiceType::KCommunicator:
        return QString("Communicator");
    case ServiceType::KPackager:
        return QString("Packager");
    default:
        return QString("Unknown");
    }
}

QVariant Settings::setting(const QString &key)
{
    return m_settings.value(key);
}

void Settings::setSetting(const QString &key, const QVariant &value)
{
    m_settings.setValue(key,value);
}
