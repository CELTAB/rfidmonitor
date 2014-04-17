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
