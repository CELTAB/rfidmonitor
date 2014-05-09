/****************************************************************************
**
** WWW.FISHMONITORING.COM.BR
**
** Copyright (C) 2013
**                     Luis Valdes <luisvaldes88@gmail.com>
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

#ifndef INTERFACES_H
#define INTERFACES_H

#include "service.h"

class Rfiddata;

class ReadingInterface : public Service
{
    Q_OBJECT
public:
    explicit ReadingInterface(QObject *parent = 0);

signals:
    /*!
     * \brief whenever new data is available and valid, this signal should emit the received data to the system.
     */
    void dataRaceived(Rfiddata *);
public slots:
    /*!
     * \brief this slots starts the reading from the serial port.
     */
    virtual void start() = 0;
    /*!
     * \brief this slot is designed to stop the reading from the serial port.
     */
    virtual void stop() = 0;
};


class PersistenceInterface : public Service
{
    Q_OBJECT
public:
    explicit PersistenceInterface(QObject *parent=0);

    virtual QList<Rfiddata *> getObjectList(const QString &ColumnObject, QVariant value, QObject *parent) = 0;
    virtual void insertObjectList(const QList<Rfiddata *> &data) = 0;
    virtual void updateObjectList(const QList<Rfiddata *> &data) = 0;
    virtual void deleteObjectList(const QList<Rfiddata *> &data) = 0;

};

class ExportInterface : public Service
{
    Q_OBJECT
public:
    explicit ExportInterface(QObject *parent = 0);

public slots:
    /*!
     * \brief when the system detects an USB storage device is inserted, it should start the exportation of the data.
     */
    virtual void startUSBExport() = 0;

    virtual void stopUSBExport() = 0;

};

class PackagerInterface : public Service
{
    Q_OBJECT
public:
    explicit PackagerInterface(QObject *parent = 0);


    /*!
     * \brief Returns a map of all packets prepared for exportation, the packets have the next format:
     * [index   ][JSON         ][MD5 Hash]
     * [ 4 bytes][variable size][16 bytes]
     * The key of the map is the MD5 Hash of the packet.
     * \return
     */
    virtual QMap<QString, QByteArray> getAll() = 0;
    virtual void update(const QList<QString> &) = 0;
    virtual void generatePackets() = 0;

public slots:


};

class SynchronizationInterface : public Service
{
    Q_OBJECT
public:
    explicit SynchronizationInterface(QObject *parent = 0);

    virtual void readyRead() = 0;
signals:
    void sendMessage(QByteArray);
public slots:
    virtual void onDataReceived(QString data) = 0;

};


class CommunicationInterface : public Service
{
    Q_OBJECT
public:
    explicit CommunicationInterface(QObject *parent=0);

signals:
    void messageReceived(QByteArray);
public slots:
    virtual void sendMessage(QByteArray) = 0;

};

#endif // INTERFACES_H
