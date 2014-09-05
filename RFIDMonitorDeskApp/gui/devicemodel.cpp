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

#include <QMapIterator>
#include <QDebug>

#include "devicemodel.h"

DeviceModel::DeviceModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

void DeviceModel::addDevice(const QString &name, const QString &macAddress, const QString &ipAddress)
{
    // Adds the device only if it does not exists yet. If already exists restart the timer.
    if(!m_deviceMap.contains(macAddress)){
        Device *d = new Device;
        d->name = name;
        d->ipAddress = ipAddress;
        d->macAddress = macAddress;
        m_deviceMap.insert(ipAddress,d);

        // When the timeout signal is emited from the timer for this device,
        //calls this lambda function to remove it from the model.
        //connect(&d->timer, &QTimer::timeout, [this,d](){this->removeDevice(d);});

        int index = m_deviceMap.keys().indexOf(ipAddress);

        //Informs the model has changed with a insert.
        beginInsertRows(QModelIndex(), index, index);
        endInsertRows();
    }else{
        //m_deviceMap.value(macAddress)->resetTimer();
    }
}

void DeviceModel::removeDevice(QString ipAddress)
{
    int index = m_deviceMap.keys().indexOf(ipAddress);

    beginRemoveRows(QModelIndex(), index, index);
    Device *device = m_deviceMap.value(ipAddress);
    m_deviceMap.remove(ipAddress);
    emit deviceRemoved(device->toString());
    delete device;

    endRemoveRows();
}

QVariant DeviceModel::deviceMacAddress(const QModelIndex &index) const
{
    if(!index.isValid())
        return QVariant();

    return m_deviceMap.values().at(index.row())->macAddress;
}

QVariant DeviceModel::deviceIPAddress(const QModelIndex &index) const
{
    if(!index.isValid())
        return QVariant();

    return m_deviceMap.values().at(index.row())->ipAddress;
}

QVariant DeviceModel::deviceName(const QModelIndex &index) const
{
    if(!index.isValid())
        return QVariant();

    return m_deviceMap.values().at(index.row())->name;
}

void DeviceModel::clear()
{
    beginResetModel();

    QMapIterator<QString, Device*> it(m_deviceMap);

    while(it.hasNext()){
        it.next();
        Device *d = it.value();
        m_deviceMap.remove(it.key());
        delete d;
    }

    endResetModel();

}

int DeviceModel::rowCount(const QModelIndex &parent) const
{
    return m_deviceMap.size();
}

QVariant DeviceModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole){
        Device *d = m_deviceMap.values().at(index.row());
        return d->toString();
    }

    return QVariant();
}

Qt::ItemFlags DeviceModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractListModel::flags(index);
}
