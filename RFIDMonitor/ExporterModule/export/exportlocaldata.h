/****************************************************************************
**
** WWW.FISHMONITORING.COM.BR
**
** Copyright (C) 2013
**                     Gustavo Valiati <gustavovaliati@gmail.com>
**                     Luis Valdes <luisvaldes88@gmail.com>
**                     Thiago R. M. Bitencourt <thiago.mbitencourt@gmail.com>
**
** This file is part of the FishMonitoring project
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

#ifndef EXPORTLOCALDATA_H
#define EXPORTLOCALDATA_H

/*!
* \class ExportLocalData
* \brief The ExportLocalData class is responsible to get the non synced data from database, and export it to a temporary file and into an external device.
*
* The ExportLocalData class have as objectives:
* get the non-synced data from the database and write it into a temporary file.
* When a device is connected it will copy the temporary file into the device and delete the file from local disk,
* change the sync flag of the temporary registers and then update the database with their current values.
* While the program is writing the exported data in the device, the application turns the red led on,
* after writing all the data the red led are turned off and the green led on until remove the device.
*/

#include <QString>
#include <QSqlDatabase>
#include <QFile>
#include <QMutex>
#include <QObject>
#include <QTimer>
#include <QThread>

class BlinkLed;
class Rfiddata;

class ExportLocalData : public QObject
{
    Q_OBJECT
public:
    explicit ExportLocalData(QObject *parent = 0);
    ~ExportLocalData();

public slots:
    /*!
     * \brief turnOffLed turns the green led off
     */
    void turnOffLed();

    /*!
     * \brief exportAction is slot that defines if the data must be export to a temporary file or into an external device.
     * If an string quals to 'temp' is received by parameter, then calls exportToTempFile() function. Otherwise, calls export to exportToDevice() function and passes the device's path to it.
     *
     * \param path. Device's path to export data. If equals 'temp' export to temporary file.
     * \sa exportToDevice() exportToTempFile()
     */
    void exportAction(QString path = "temp");

    /*!
     * \brief startExport is the function that starts this class like thread and starts a QTimer to export temporary file
     */
    void startExport();

private:
    /*!
     * \brief When a QTimer emmit a timeoutr signal this function is called to export the non sync data into a temporary file. It will search for data and if existe non cync data it will export to file and return. Just return, otherwise.
     */
    bool exportToTempFile();

    /*!
     * \brief exportToDevice receive device's path to copy file into it.
     * Once the data is successfully exported, it will delete the temporary file from local disk.
     */
    bool exportToDevice(QString device);

    // Name of the module. Is used to write log records
    QString m_module;

    /*!
     * \brief m_exportTime is a QTimer that defines the interval of exportation to temp file. The timeout signal is connected to exportAction() slot.
     *
     * \sa exporAction()
     */
    QTimer *m_exportTime;

    // Define the temporary file to export data
    QFile m_tempFile;
    QMutex m_mutex;

    QString m_fileName;

    /*!
     * \brief m_blickLed is an object to manipulate the green and red leds, which are used to show to the user the status of process
     */
    BlinkLed *m_blinkLed;

    // Time that define the interval to export data to temporary file
    int exportTime = 1000*60;
};

#endif // EXPORTLOCALDATA_H
