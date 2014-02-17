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


QFile file_hard("captured_hard.txt");
QFile file_matched("captured_matched.txt");

DataReader::DataReader(QObject *parent) :
    QObject(parent),
    m_serial(new QSerialPort(this))
{
    m_module = "ReadingModule_MRI2000";

    connect(m_serial, SIGNAL(readyRead()), SLOT(readData()));
    connect(m_serial, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(handleError(QSerialPort::SerialPortError)));


    if (file_hard.open(QFile::WriteOnly | QFile::Append)){
        m_outHardCapture.setDevice(&file_hard);
    }
    if (file_matched.open(QFile::WriteOnly | QFile::Append)){
        m_outMatchedCapture.setDevice(&file_matched);
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
        //        QTimer::singleShot(300, QCoreApplication::instance(), SLOT(quit()));
        return false;
    }
    return true;
}

void DataReader::readData()
{

    if(m_serial->bytesAvailable() >= 64){
        QByteArray buffer = m_serial->readAll();

        QString hardData(buffer);

//        if(m_outHardCapture.device()){
//            m_outHardCapture << hardData;
//            m_outHardCapture.flush();
//        }

        hardData.remove(QRegExp("[\\n\\t\\r]"));

        QRegularExpression regexCode;
        regexCode.setPattern("TAG[0-9a-fA-F]W\\s[0-9a-fA-F]{3}\\s[0-9a-fA-F]{16}");
        QRegularExpressionMatch match = regexCode.match(hardData);

        if(match.hasMatch()) {

            //Here the matched string must be like "TAG5W 001 0000000295901506"

//            if(m_outMatchedCapture.device()){
//                m_outMatchedCapture << match.captured(0);
//                m_outMatchedCapture.flush();
//            }

            Rfiddata *data = new Rfiddata(this);

            //Temporary define a static id to the collect point.
            int idPontoColeta = 1;
            data->setIdpontocoleta(idPontoColeta);

            //The character 3 from string is the number of antenna: TAG[5]W...
            data->setIdantena(match.captured(0).at(3).digitValue());

            //Try to convert the code of 16 characters, from hexa to decimal.
            bool hexaConvertion;
            QString hexaCode(match.captured(0).right(16));
            QString deciCode(QString::number(hexaCode.toLongLong(&hexaConvertion, 16)));

            if(!hexaConvertion){
                //Problem converting from hexadecimal.
                Logger::instance()->writeRecord(Logger::fatal, m_module, Q_FUNC_INFO, QString("Could not convert RFID code from hexadecimal. Hexa code: "+hexaCode));
                return;
            }

            //From the full code, the leftmost 4 are the application core.
            data->setApplicationcode(deciCode.left(4));
            //From the full code, removing the application code, there is the identification code
            data->setIdentificationcode(deciCode.remove(deciCode.left(4)));
            //Take the current date and set on the object
            data->setDatetime(QDateTime::currentDateTime());
            //Set the object as NotSynced
            data->setSync(Rfiddata::KNotSynced);

            try{
                std::function< bool (Rfiddata *)> persistenceFunc = ServiceManager::instance()->get_function< bool, Rfiddata *>("persistence.insert_object");
                persistenceFunc(data);
            }catch(std::exception &ex){
                Logger::instance()->writeRecord(Logger::fatal, m_module, Q_FUNC_INFO, QString("could not insert object on database").arg(ex.what()));
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
