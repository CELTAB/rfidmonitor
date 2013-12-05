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
#include <QSerialPortInfo>
#include <QDebug>

#include "datareader.h"

DataReader::DataReader(QObject *parent) :
    QObject(parent),
    m_serial(new QSerialPort(this))
{
    connect(m_serial, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(handleError(QSerialPort::SerialPortError)));
}

DataReader::~DataReader()
{
    if(m_serial->isOpen()){
        m_serial->close();
    }
}

bool DataReader::isOpen()
{
    return m_serial->isOpen();
}

bool DataReader::openDevice(const QString &device)
{
    QSerialPortInfo info(device);
    m_serial->setPort(info);
    m_serial->setBaudRate(QSerialPort::Baud9600);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setStopBits(QSerialPort::OneStop);
    m_serial->setParity(QSerialPort::NoParity);

    if(!m_serial->open(QIODevice::ReadWrite)){
        qDebug() << "Could not open device " << device;
        return false;
    }
    return true;
}

bool DataReader::closeDevice()
{
    if(m_serial->isOpen()){
        m_serial->close();
        return true;
    }
    return false;
}

void DataReader::writeData(QByteArray data)
{
    if(m_serial->isOpen()){
        m_serial->write(data.data());
    }
}

void DataReader::handleError(QSerialPort::SerialPortError error)
{
    if(error != QSerialPort::NoError){
        qDebug() << QString("Error: %1").arg(m_serial->errorString());
    }
}
