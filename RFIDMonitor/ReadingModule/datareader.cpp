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
#include <QFile>
#include <QTextStream>

#include <iostream>
#include <ctime>

#include "datareader.h"

#include <logger.h>
#include <object/rfiddata.h>
#include <servicemanager.h>


QFile file("rfidmonitor.txt");
QFile fileCaptured("rfidmonitor_captured.txt");

DataReader::DataReader(QObject *parent) :
    ReadingInterface(parent),
    m_serial(0)
{
    m_module = "ReadingModule";
    m_serial = new QSerialPort(this);

    connect(m_serial, SIGNAL(readyRead()), SLOT(readData()));
    connect(m_serial, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(handleError(QSerialPort::SerialPortError)));


    if (file.open(QFile::WriteOnly | QFile::Append)){
        m_outReceived.setDevice(&file);
    }
    if (fileCaptured.open(QFile::WriteOnly | QFile::Append)){
        m_outCaptured.setDevice(&fileCaptured);
    }
}

DataReader::~DataReader()
{
    if (m_serial->isOpen()) {
        m_serial->close();
    }
}

bool DataReader::startReading(const QString &device)
{
    if(!m_serial->open(QIODevice::ReadWrite))
    {

        Logger::instance()->writeRecord(Logger::fatal, m_module, Q_FUNC_INFO, QString("Could not open device %1").arg(device));
        // create class invalid_device exception on core Module
        QTimer::singleShot(300, QCoreApplication::instance(), SLOT(quit()));
    }else{
        QSerialPortInfo info(device);
        m_serial->setPort(info);
        m_serial->setBaudRate(QSerialPort::Baud9600);
        m_serial->setDataBits(QSerialPort::Data8);
        m_serial->setStopBits(QSerialPort::OneStop);
        m_serial->setParity(QSerialPort::NoParity);
    }
    return false;
}

void DataReader::readData()
{
    static QByteArray lastBuffer;
    if(m_serial->bytesAvailable() >= 64){
        QByteArray buffer = m_serial->readAll();
        QRegExp regex;
        regex.setPattern("(L(\\d{2})?W)?(\\s?)[0-9a-fA-F]{4}(\\s)?[0-9a-fA-F]{16}");

        QString data(lastBuffer + buffer);

        if(m_outReceived.device()){
            m_outReceived << data;
            m_outReceived.flush();
        }

        lastBuffer = buffer;

        data.remove(QRegExp("[\\n\\t\\r]"));

        int pos = regex.indexIn(data);
        if(pos != -1){
            // The first string in the list is the entire matched string.
            // Each subsequent list element contains a string that matched a
            // (capturing) subexpression of the regexp.
            QStringList matches = regex.capturedTexts();

            // crear RFIDData
            QRegularExpression regexCode;
            regexCode.setPattern("[0-9a-fA-F]{4}(\\s)?[0-9a-fA-F]{16}");
            QRegularExpressionMatch match = regexCode.match(matches.at(0));

            if(m_outCaptured.device()){
                m_outCaptured << matches.at(0);
                m_outCaptured.flush();
            }


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
