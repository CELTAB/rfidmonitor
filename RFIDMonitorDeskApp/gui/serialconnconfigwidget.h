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

#ifndef SERIALCONNCONFIGWIDGET_H
#define SERIALCONNCONFIGWIDGET_H

#include <QWidget>

#include "communication/serialcommunication.h"

namespace Ui {
class SerialConnConfigWidget;
}

class SerialConnConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SerialConnConfigWidget(QWidget *parent = 0);
    ~SerialConnConfigWidget();

private:
    Ui::SerialConnConfigWidget *ui;

    /**
     * @brief refreshDeviceList offers the way to refresh the list of available connected devices.
     */
    void refreshDeviceList();

    /**
     * @brief connectToDevice offers the way to connect to the device, defined with the connection
     * parameters by the user in the window.
     */
    void connectToDevice();

    /**
     * @brief populateFields load the serial communication options in the window for the user.
     */
    void populateFields();

public slots:
    /**
     * @brief btRefreshDeviceClicked is a slot that calls the refreshDeviceList when the user ask for it
     * in the window button.
     */
    void btRefreshDeviceClicked();

    /**
     * @brief btConnectToDeviceClicked is a slot that calls connectToDevice when the user ask for it
     * in the window button.
     */
    void btConnectToDeviceClicked();

signals:
    /**
     * @brief serialCommunicationReady is a signal emmited when the connection is ready.
     */
    void serialCommunicationReady();
};

#endif // SERIALCONNCONFIGWIDGET_H
