#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
#include <QVariant>
#include <QMapIterator>
#include <QDebug>

#include "raspconfigurationjson.h"

RaspConfigurationJson::RaspConfigurationJson(QObject *parent) :
    QObject(parent)
{
    id = "0";
    ip = "0";
    name = "0";
    defaultServices = QMap<QString, QString>();
    defaultServices.insert("reading", "0");
    defaultServices.insert("persistence", "0");
    defaultServices.insert("export", "0");
    defaultServices.insert("communication", "0");
}
QString RaspConfigurationJson::getId() const
{
    return id;
}

void RaspConfigurationJson::setId(const QString &value)
{
    id = value;
}
QString RaspConfigurationJson::getIp() const
{
    return ip;
}

void RaspConfigurationJson::setIp(const QString &value)
{
    ip = value;
}
QString RaspConfigurationJson::getName() const
{
    return name;
}

void RaspConfigurationJson::setName(const QString &value)
{
    name = value;
}
QMap<QString, QString> RaspConfigurationJson::getDefaultServices() const
{
    return defaultServices;
}

void RaspConfigurationJson::setDefaultServices(const QMap<QString, QString> &value)
{
    defaultServices = value;
}

void RaspConfigurationJson::insertDefaultService(const QString &serviceType, const QString &serviceName)
{
    defaultServices.insert(serviceType, serviceName);
}

QByteArray RaspConfigurationJson::generateJson()
{
    QJsonDocument masterDoc;
    QJsonObject rootObj;

    rootObj.insert("id", QJsonValue(this->getId()));
    rootObj.insert("name", QJsonValue(this->getName()));
    rootObj.insert("ip", QJsonValue(this->getIp()));

    QJsonObject defaultServicesObj;
    QMapIterator <QString, QString> servicesIterator(getDefaultServices());
    while(servicesIterator.hasNext()){
        servicesIterator.next();
        defaultServicesObj.insert(servicesIterator.key(), servicesIterator.value());
    }
    rootObj.insert("defaultServices", QJsonValue(defaultServicesObj));

    masterDoc.setObject(rootObj);
    return masterDoc.toJson();

}

bool RaspConfigurationJson::loadJson(QByteArray jsonFile)
{
    QJsonParseError parseError;
    QJsonDocument masterDoc(QJsonDocument::fromJson(jsonFile, &parseError));
    if(masterDoc.isNull()){
        qDebug() << "Parse error:" << parseError.errorString();
        return false;
    }
    QJsonObject rootObj(masterDoc.object());

    if( ! rootObj.value("id").isUndefined())
        setId(rootObj.value("id").toString());
    if( ! rootObj.value("ip").isUndefined())
        setIp(rootObj.value("ip").toString());
    if( ! rootObj.value("name").isUndefined())
        setName(rootObj.value("name").toString());

    if( ! rootObj.value("defaultServices").isUndefined()){
        QJsonObject defaultServices(rootObj.value("defaultServices").toObject());
        if( ! defaultServices.value("communication").isUndefined())
            insertDefaultService("communication", defaultServices.value("communication").toString());
        if( ! defaultServices.value("export").isUndefined())
            insertDefaultService("export", defaultServices.value("export").toString());
        if( ! defaultServices.value("persistence").isUndefined())
            insertDefaultService("persistence", defaultServices.value("persistence").toString());
        if( ! defaultServices.value("reading").isUndefined())
            insertDefaultService("reading", defaultServices.value("reading").toString());
    }


    return true;

}




