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

#ifndef DEVICEMODEL_H
#define DEVICEMODEL_H

#include <QAbstractListModel>
#include <QTimer>
#include <QList>

/**
 * @brief The Device struct represents the Raspberry device object.
 */
struct Device
{
    Device()
    {

    }

    ~Device()
    {

    }

    /**
     * @brief operator == Overload the ==operator to compare with the macAddress value.
     * @param value is the macAddress to be compared.
     * @return true if equals, false otherwise.
     */
    bool operator==(const QString &value) const
    {
        return ipAddress==value;
    }

    /**
     * @brief toString prints in a pattern the state of the object.
     * @return the QString with "MAC - IP:Port" format"
     */
    QString toString()
    {
        return QString("Name: %1 | MAC: %2 | IP: %3").arg(name).arg(macAddress).arg(ipAddress);
    }

    QString macAddress;
    QString ipAddress;
    QString name;
};

/**
 * @brief The DeviceModel class is the list model to be used in the QListView that represents a list of Devices.
 */
class DeviceModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit DeviceModel(QObject *parent = 0);

    /**
     * @brief reimplemented function from QAbstractListModel.
     */
    void addDevice(const QString &name, const QString &macAddress, const QString &ipAddress);

    /**
     * @brief reimplemented function from QAbstractListModel.
     */
    void removeDevice(QString ipAddress);

    /**
     * @brief reimplemented function from QAbstractListModel.
     */
    int rowCount(const QModelIndex &parent) const;

    /**
     * @brief reimplemented function from QAbstractListModel.
     */
    QVariant data(const QModelIndex &index, int role) const;

    /**
     * @brief reimplemented function from QAbstractListModel.
     */
    Qt::ItemFlags flags(const QModelIndex &index) const;


    /**
     * @brief deviceMacAddress gets the MAC address for the device of the passed index.
     * @param index refers the requested device index.
     * @return QVariant with MAC Address.
     */
    QVariant deviceMacAddress(const QModelIndex &index) const;

    /**
     * @brief deviceIPAddress gets the IP address for the device of the passed index.
     * @param index refers the requested device index.
     * @return QVariant with IP address.
     */
    QVariant deviceIPAddress(const QModelIndex &index) const;

    /**
     * @brief deviceDaemonPort gets the daemon port for the device of the passed index.
     * @param index refers the requested device index.
     * @return QVariant with daemon port.
     */
    QVariant deviceName(const QModelIndex &index) const;

    /**
     * @brief clear remove all devices from the model.
     */
    void clear();

private:
    /**
     * @brief m_deviceMap is a map that holds the list of devices, by a key as MAC Address.
     */
    QMap<QString,Device*> m_deviceMap;

signals:
    /**
     * @brief deviceRemoved is a signal that informs a removed device from the model.
     */
    void deviceRemoved(QString);
};

#endif // DEVICEMODEL_H
