/****************************************************************************
**
** www.celtab.org.br
**
** Copyright (C) 2013, 2014
**                     Luis Valdes <luisvaldes88@gmail.com>
**
** This file is part of the RFIDMonitor project
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; version 2
** of the License.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**
****************************************************************************/

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
    ~ConfigManager();

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
    void restoreConfig();

private:
    QFile m_json;
    QFile m_interfaces;
    QString m_hostName;

    int m_hostPort;
    bool m_restartNetwork;

    json::RFIDMonitorSettings m_systemSettings;
    json::RFIDMonitorSettings m_backupSettings;
    void openJsonFile();
    bool saveJsonFile();

};

#endif // CONFIGMANAGER_H
