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
#include <QRegExp>
#include <QStringList>
#include <QSerialPortInfo>
#include <QDateTime>
#include <QCoreApplication>
#include <QRegularExpression>
#include <QTimer>

#include <QDir>
#include <iostream>
#include <logger.h>

#include <ctime>

#include "datareader.h"

#include <object/rfiddata.h>
#include <servicemanager.h>


DataReader::DataReader(QObject *parent) :
    QObject(parent),
    m_serial(new QSerialPort(this))
{
    m_module = "ReadingModule";

    connect(m_serial, SIGNAL(readyRead()), SLOT(readData()));
    connect(m_serial, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(handleError(QSerialPort::SerialPortError)));
}

DataReader::~DataReader()
{
    if (m_serial->isOpen()) {
        m_serial->close();
    }
}

bool DataReader::startReading(const QString &device)
{
    QSerialPortInfo info(device);
    m_serial->setPort(info);
    m_serial->setBaudRate(QSerialPort::Baud9600);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setStopBits(QSerialPort::OneStop);
    m_serial->setParity(QSerialPort::NoParity);

    if(!m_serial->open(QIODevice::ReadWrite))
    {
       Logger::instance()->writeRecord(Logger::fatal, m_module, Q_FUNC_INFO, QString("Could not open device %1").arg(device));
        // create class invalid_device exception on core Module
        QTimer::singleShot(300, QCoreApplication::instance(), SLOT(quit()));
    }
    return false;
}

void DataReader::readData()
{
    if(m_serial->bytesAvailable() >= 64){
        QByteArray buffer = m_serial->readAll();
        QRegExp regex;
        regex.setPattern("(LW)(\\s?)([0-9]{4})(\\s?)([0-9]{16})");

        QString data(buffer);

        data.remove(QRegExp("[\\n\\t\\r]"));

        int pos = regex.indexIn(data);
        if(pos != -1){
            // The first string in the list is the entire matched string.
            // Each subsequent list element contains a string that matched a
            // (capturing) subexpression of the regexp.
            QStringList matches = regex.capturedTexts();

            // crear RFIDData
            QRegularExpression regexCode;
            regexCode.setPattern("([0-9]{4})(\\s?)([0-9]{16})");
            QRegularExpressionMatch match = regexCode.match(matches.at(0));

            if(match.hasMatch()) {
                Rfiddata *data = new Rfiddata(this);

                int idPontoColeta = 1;
                data->setIdpontocoleta(idPontoColeta);

                int idAntena = 1;
                data->setIdantena(idAntena);

                qlonglong applicationcode = match.captured(1).toLongLong();
                qlonglong identificationcode = match.captured(3).toLongLong();

                data->setApplicationcode(applicationcode);
                data->setIdentificationcode(identificationcode);
                data->setDatetime(QDateTime::currentDateTime());
                data->setSync(Rfiddata::KNotSynced);

//                Logger::instance()->writeRecord(Logger::debug, m_module, Q_FUNC_INFO, QString("%1").arg(match.captured(0)));
                try{
                    std::function< bool (Rfiddata *)> persistenceFunc = ServiceManager::instance()->get_function< bool, Rfiddata *>("persistence.insert_object");
                    persistenceFunc(data);
                }catch(std::exception &ex){
                    Logger::instance()->writeRecord(Logger::fatal, m_module, Q_FUNC_INFO, QString("could not insert object on database").arg(ex.what()));
//                    Logger::instance()->writeRecord(Logger::debug, m_module, Q_FUNC_INFO, QString("could not insert object on database %1").arg(ex.what()));
                }
            }
        }
    }
}

void DataReader::handleError(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError)
    {
        Logger::instance()->writeRecord(Logger::error, m_module, Q_FUNC_INFO, QString("Error: %1").arg(m_serial->errorString()));
    }
}
