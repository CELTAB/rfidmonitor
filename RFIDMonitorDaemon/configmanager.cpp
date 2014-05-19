#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QNetworkInterface>

#include "configmanager.h"

ConfigManager::ConfigManager(QObject *parent):
    QObject(parent)
{

    m_restartNetwork = false;

    m_json.setFileName("rfidmonitor.json");
    m_interfaces.setFileName("networkInterfaces");
    openJsonFile();
}

ConfigManager::~ConfigManager()
{
    delete &m_systemSettings;
}

bool ConfigManager::setDateTime(QDateTime datetime)
{
    bool status = true;
    //        date -s 2014-04-10T09:23:28  --  Exemplo.
    QString dateCommand("date -s ");
    dateCommand.append(datetime.toString(Qt::ISODate));

    //        Change Date/Time on Raspi.
    QProcess p;
    p.start(dateCommand);
    status = p.waitForFinished();
    return status;
}

QJsonObject ConfigManager::currentConfig()
{
    QJsonObject currentConfig;
    m_systemSettings.write(currentConfig);
    return currentConfig;
}

QJsonObject ConfigManager::identification()
{
    QJsonObject basicConfig;

    basicConfig.insert("id", QJsonValue(m_systemSettings.id()));
    basicConfig.insert("name", m_systemSettings.name());
    basicConfig.insert("macaddress", m_systemSettings.macAddress());

    return basicConfig;
}

void ConfigManager::setIdentification(QJsonObject &json)
{
#if QT_VERSION < 0x050200
    m_systemSettings.setId(json.value("id").toVariant().toInt());
#else
    m_systemSettings.setId(json.value("id").toInt());
#endif // QT_VERSION < 0x050200

    //    m_systemSettings.setMacAddress(json.value("macaddress").toString());
    m_systemSettings.setName(json.value("name").toString());
#if QT_VERSION < 0x050200
    m_systemSettings.setId(json.value("id").toVariant().toInt());
#else
#endif // QT_VERSION < 0x050200
    if(saveJsonFile()){
        // qDebug() << "New identification saved successfully";
    }
}

QString ConfigManager::hostName()
{
    return m_systemSettings.serverAddress();
}

int ConfigManager::hostPort()
{
    return m_systemSettings.serverPort();
}

void ConfigManager::openJsonFile()
{
    if (!m_json.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
    }

    QByteArray saveData = m_json.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    m_systemSettings.read(loadDoc.object());
    // get the mac address
    m_systemSettings.setMacAddress(QNetworkInterface().allInterfaces().at(1).hardwareAddress());
    m_backupSettings = m_systemSettings;
    m_json.close();
}

bool ConfigManager::saveJsonFile()
{
    if (!m_json.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }

    QJsonObject obj;
    m_systemSettings.write(obj);
    QJsonDocument saveDoc(obj);
    if(m_json.write(saveDoc.toJson()) == -1)
        return false;
    m_json.close();
    return true;
}

bool ConfigManager::newConfig(QJsonObject &json)
{
    QJsonObject network = json["network"].toObject();

    if(m_systemSettings.networkConfiguration().essid() != network.value("essid").toString()
            || m_systemSettings.networkConfiguration().password() != network.value("password").toString()){

        qDebug() << "Must to restart Network. Only after restart Monitor";
        setNetConfig(network);
        m_restartNetwork = true;
    }

    m_systemSettings.read(json);
    return saveJsonFile();
}

void ConfigManager::restoreConfig()
{
    m_systemSettings = m_backupSettings;
    saveJsonFile();
}

QJsonObject ConfigManager::netConfig()
{
    QJsonObject netConfig;

    netConfig.insert("essid", m_systemSettings.networkConfiguration().essid());
    netConfig.insert("password", m_systemSettings.networkConfiguration().password());

    return netConfig;
}

bool ConfigManager::setNetConfig(QJsonObject &netJson)
{
    json::Network network;
    network.read(netJson);

    m_systemSettings.setNetworkConfiguration(network);
    return saveJsonFile();
}

bool ConfigManager::restartNetwork()
{

    if(m_restartNetwork){

        qDebug() << "Restarting Network. You will lose connection... ";

        if (!m_interfaces.open(QIODevice::WriteOnly)) {
            qWarning("Couldn't open file.");
            return false;
        }

        m_restartNetwork = false;
        static QString interface("auto lo\n"
                                 "iface lo inet loopback\n\n"
                                 "auto eth0\n"
                                 "iface eth0 inet dhcp\n\n"
                                 "auto wlan0\n"
                                 "iface wlan0 inet dhcp\n"
                                 "\twpa-essid %1\n"
                                 "\twpa-psk %2\n");

        //                             "iface eth0 inet static\n"
        //                             "\taddress 192.168.1.5\n"
        //                             "\tnetmask 255.255.255.0\n"
        //                             "\tgateway 192.168.1.254\n"

        m_interfaces.write(interface.arg(m_systemSettings.networkConfiguration().essid()).arg(m_systemSettings.networkConfiguration().password()).toLatin1());
        m_interfaces.close();

        QProcess *restartNet = new QProcess;
        connect(restartNet, ( void (QProcess::*)(int))&QProcess::finished, [=](int){
            qDebug() << "Delete: QProcess *restartNet" ;
            restartNet->deleteLater();
        });
        restartNet->start("service networking restart");
    }
    return true;
}
