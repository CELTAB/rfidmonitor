#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QProcess>
#include <QDateTime>
#include <QDebug>
#include <QByteArray>

#include <QJsonDocument>
#include <QJsonObject>
#include <CoreLibrary/json/rfidmonitorsettings.h>

#include <QFile>

class ConfigManager : public QObject
{
    Q_OBJECT

public:
    explicit ConfigManager(QObject *parent = 0);

    bool setDateTime(QDateTime datetime);
    QJsonObject currentConfig();
    QJsonObject identification();
    void setIdentification(QJsonObject &json);

    QString hostName();
    int hostPort();
    bool newConfig(QJsonObject &json);
    QJsonObject netConfig();
    bool setNetConfig(QJsonObject &netJson);
    bool restartNetwork();

private:
    QFile m_json;
    QFile m_interfaces;
    QString m_hostName;
    int m_hostPort;

    json::RFIDMonitorSettings m_systemSettings;
    void openJsonFile();
    bool saveJsonFile();

};

#endif // CONFIGMANAGER_H
