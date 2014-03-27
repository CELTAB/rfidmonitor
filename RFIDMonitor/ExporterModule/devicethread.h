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

#ifndef DEVICETHREAD_H
#define DEVICETHREAD_H

#include <QThread>

#include "export/exportlocaldata.h"

class ExportLocalData;

class DeviceThread : public QObject
{
    Q_OBJECT

public:
    explicit DeviceThread(QObject *parent = 0);
    static DeviceThread * instance();

signals:
    /*!
     * \brief exportToDevice is a signal to export data into the external device just connected. Passes the path by parameter or an empty string otherwise
     * \param device
     */
    void exportToDevice(QString device);

    /*!
     * \brief redLedOff emit a signal to turn off the green led
     */
    void turnLedOff();

public slots:
    /*!
     * \brief startListening is the function that starts this thread and starts the loop to listening new devices connection
     */
    void startListening();
};

#endif // DEVICETHREAD_H
