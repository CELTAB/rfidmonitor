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
* \brief The ExportLocalData class is responsible to get the non synced data from database, and export it to device.
*
* The ExportLocalData class has as objectives:
* get the path to the device just connected to the computer,
* get the non-synced data from the database,
* write these data into a file and save it in the device,
* change the sync flag of the temporary registers and then update the database with their current values.
* While the program is writing the exported data in the device, the application turns the red led on,
* after writing all the data the red led are turned off and the green led on until remove the device.
*/

#include <QString>
#include <QSqlDatabase>
#include <QMutex>
#include <QObject>

class BlinkLed;
class Rfiddata;

class ExportLocalData : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief instance is the singleton
     * \return the unique instance of the ExportLocalData class
     */
    static ExportLocalData * instance();

    /*!
     * \brief exportAllNonSyncedRegisters get the path of the device on which the data will be exported. Passes the device path to the exportToDevice function
     * \return true if the data was exported successfully and false otherwise
     * \sa exportToDevice
     */
    bool exportAllNonSyncedRegisters();

    /*!
     * \brief exportCSVData write a file with CSV format
     * \param device path where the file must be saved
     * \param list of data to be exported
     */
    void exportCSVData(const QString &device, const QList<Rfiddata *> &list) throw(std::exception);

    /*!
     * \brief exportLegacyCSVData write a file with CSV format, but using a legacy pattern
     * \param device path where the file must be saved
     * \param list of data to be exported
     */
    void exportLegacyCSVData(const QString &device, const QList<Rfiddata *> &list) throw(std::exception);

    /*!
     * \brief turnOffLed turns the green led off
     */
    void turnOffLed();


private:
    QString m_module;
    explicit ExportLocalData(QObject *parent = 0);

    /*!
     * \brief exportToDevice get a list of non-synced data from the database and passes to the exportCSVData and exportLegacyCSVData functions, also passes the device path.
     * Once the data is successfully exported, it will change the sync flag of the temporary registers and then update the database with their current values.
     *
     * \param device path where the data must be exported
     * \return true if the data was successfully exported and false otherwise
     * \sa exportLegacyCSVData exportCSVData
     */
    bool exportToDevice(QString device);
    QMutex m_mutex;

    /*!
     * \brief m_blickLed is an object to manipulate the green and red leds, which are used to show to the user the status of process
     */
    BlinkLed *m_blickLed;

};

#endif // EXPORTLOCALDATA_H
