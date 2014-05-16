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

#ifndef RFIDMONITOR_H
#define RFIDMONITOR_H

#include <QObject>
#include <QMap>
#include <QMutex>

class QCoreApplication;
class QThread;

struct RFIDMonitorPrivate;
class CoreModule;
enum class ServiceType;
class Service;

/*!
 * \brief The RFIDMonitor class is the one in charge of loading all the modules and calling the main service module.
 */
class RFIDMonitor : public QObject
{
    Q_OBJECT
public:

    static RFIDMonitor * instance();
    ~RFIDMonitor();
    
    /*!
     * \brief isconnected returns true if there's connection with a server. False otherwaise.
     * The status is defined by message received from RFIDMonitorDaemon
     */
    bool isconnected();

    /*!
     * \brief idCollector gets the collector's id from the configuration file
     * \return the collector's id
     */
    int idCollector();

    /*!
     * \brief collectorName gets the name from configuration file
     * \return the name of this collector.
     */
    QString collectorName();

    /*!
     * \brief start loads the modules of the system, initialize them and then calls the main service.
     * \param app is used to get the parameters of the application
     */
    void start(const QCoreApplication &app);

    const QList<CoreModule *> & moduleList() const;
    QList<Service *> services(ServiceType type);
    Service * defaultService(ServiceType type);

    QVariant getProperty(const QString &propName);

    /*!
     * \brief setDefaultService TEMP
     * \param type
     */
    void setDefaultService(ServiceType type, QString name);

    bool isRunning();

    QString device();

public slots:
    void stop();
    void newMessage(QByteArray message);

private:
    explicit RFIDMonitor(QObject *parent = 0);
    RFIDMonitorPrivate *d_ptr;
};

#endif // RFIDMONITOR_H
