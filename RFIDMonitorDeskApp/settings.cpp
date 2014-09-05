/****************************************************************************
**
** www.celtab.org.br
**
** Copyright (C) 2013
**                     Gustavo Valiati <gustavovaliati@gmail.com>
**                     Luis Valdes <luisvaldes88@gmail.com>
**                     Thiago R. M. Bitencourt <thiago.mbitencourt@gmail.com>
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
