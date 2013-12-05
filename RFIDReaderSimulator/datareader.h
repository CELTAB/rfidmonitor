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

#ifndef DATAREADER_H
#define DATAREADER_H

#include <QObject>
#include <QSerialPort>

class Rfiddata;
class DeviceThread;

class DataReader : public QObject
{
    Q_OBJECT
public:
    explicit DataReader(QObject *parent = 0);
    ~DataReader();

    bool isOpen();
    bool openDevice(const QString &device);
    bool closeDevice();
private:
    QSerialPort *m_serial;

public slots:
    void writeData(QByteArray data);
    void handleError(QSerialPort::SerialPortError error);
};

#endif // DATAREADER_H
