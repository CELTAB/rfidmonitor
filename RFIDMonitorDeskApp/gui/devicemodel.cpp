#include <QMapIterator>
#include <QDebug>

#include "devicemodel.h"

DeviceModel::DeviceModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

void DeviceModel::addDevice(const QString &macAddress, const QString &ipAddress, const int &daemonPort)
{
    if(!m_deviceMap.contains(macAddress)){
        Device *d = new Device;
        d->daemonPort = daemonPort;
        d->ipAddress = ipAddress;
        d->macAddress = macAddress;
        m_deviceMap.insert(macAddress,d);
        connect(&d->timer, &QTimer::timeout, [this,d](){this->removeDevice(d);});

        int index = m_deviceMap.keys().indexOf(macAddress);

        beginInsertRows(QModelIndex(), index, index);
        endInsertRows();
    }else{
        m_deviceMap.value(macAddress)->resetTimer();
    }
}

void DeviceModel::removeDevice(Device *device)
{
    int index = m_deviceMap.keys().indexOf(device->macAddress);

    beginRemoveRows(QModelIndex(), index, index);

    m_deviceMap.remove(device->macAddress);
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

QVariant DeviceModel::deviceDaemonPort(const QModelIndex &index) const
{
    if(!index.isValid())
        return QVariant();

    return m_deviceMap.values().at(index.row())->daemonPort;
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
