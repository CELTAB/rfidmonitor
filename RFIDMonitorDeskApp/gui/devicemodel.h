#ifndef DEVICEMODEL_H
#define DEVICEMODEL_H

#include <QAbstractListModel>
#include <QTimer>
#include <QList>

struct Device
{
    Device()
    {
        timer.setSingleShot(true);
        timer.setInterval(10000);
        timer.start();
    }

    ~Device()
    {

    }

    void resetTimer()
    {
        timer.start();
    }


    bool operator==(const QString &value) const
    {
        return macAddress==value;
    }

    QString toString()
    {
        return QString("MAC: %1 - IP:Port %2:%3").arg(macAddress).arg(ipAddress).arg(daemonPort);
    }


    QString macAddress;
    QString ipAddress;
    int daemonPort;
    QTimer timer;


};

class DeviceModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit DeviceModel(QObject *parent = 0);
    void addDevice(const QString &macAddress, const QString &ipAddress, const int &daemonPort);
    void removeDevice(Device *device);
    QVariant deviceMacAddress(const QModelIndex &index) const;
    QVariant deviceIPAddress(const QModelIndex &index) const;
    QVariant deviceDaemonPort(const QModelIndex &index) const;
    void clear();

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    QMap<QString,Device*> m_deviceMap;

signals:
    void deviceRemoved(QString);
};

#endif // DEVICEMODEL_H
