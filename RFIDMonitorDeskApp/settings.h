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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QVariant>

class Settings
{
private:
    QSettings m_settings;
    Settings();
public:
    enum ConnectionType {KSerial = 0, KNetwork};
    enum TcpDataType {KHandshake = 0,
                      KConfiguration,
                      KNewReaderCommand,
                      KNewReaderAnswer,
                      KDidntUnderstand,
                      KInstruction,
                      KRFIDMonitorAnswer
                     };
    enum InteractionType {KReader = 0, KRFIDMonitor};
    enum ServiceType {
        KReaderService = 1,
        KPersister,
        KExporter,
        KSynchronizer,
        KCommunicator,
        KPackager
    };

    static Settings *instance();
    QString stringifyServiceType(Settings::ServiceType type);

    QVariant setting(const QString &key);
    void setSetting(const QString &key, const QVariant &value);
};

#endif // SETTINGS_H
