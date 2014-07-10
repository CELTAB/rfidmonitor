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

#ifndef READER_MRI2000_H
#define READER_MRI2000_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QSerialPort>
#include <QTimer>

#include <algorithm>
#include <core/interfaces.h>

class Rfiddata;
class DeviceThread;

class Reader_MRI2000 : public ReadingInterface
{
    Q_OBJECT
    
public:
    explicit Reader_MRI2000(QObject *parent = 0);
    ~Reader_MRI2000();

    QString serviceName() const;
    void init();
    ServiceType type();

    void fullRead(bool fr);
    void write(QString command);

private:
    int idCollector;
    bool allLines;
    QString m_module;
    QSerialPort *m_serial;
    QMap<qlonglong, QTimer*> m_map;

public slots:
    void readData();
    void handleError(QSerialPort::SerialPortError error);

    void start();
    void stop();

signals:
    void rfidReaded(Rfiddata *data);
};

#endif // READER_MRI2000_H
