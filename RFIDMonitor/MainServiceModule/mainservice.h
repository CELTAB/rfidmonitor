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

#ifndef MAINSERVICE_H
#define MAINSERVICE_H

#include <QObject>

/*!
 * \brief The MainService class goal is to register a callable function that invokes the "reading service" and "export daemon"
 */
class MainService : public QObject
{
    Q_OBJECT
public:
    static const QString KDeviceParam;

    explicit MainService(QObject *parent = 0);

    /*!
     * \brief callMainServices is the method that invokes the services, the argument of the method is a map that contains te parameters of the device for future specific implementations.
     * \param services
     */
    void callMainServices(const QMap< QString, QString> &services);

private slots:
    /*!
     * \brief retryStartReading if the system could not open the serial port, it tries to open it again after 6 seconds.
     */
    void retryStartReading();

private:
    QString m_module;
    QString m_device;
};

#endif // MAINSERVICE_H
