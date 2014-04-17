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
        //timer.setSingleShot(true); /**< Set the timeout timer to execute only one time. */
        //timer.setInterval(10000); /**< Set the timeout period to 10 seconds. */
        //timer.start(); /**< Start the timeout timer when the object is created here.*/
    }

    ~Device()
    {

    }

    /**
     * @brief resetTimer restarts the timer.
     */
//    void resetTimer()
//    {
//        timer.start();
//    }

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
//    QTimer timer;
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
